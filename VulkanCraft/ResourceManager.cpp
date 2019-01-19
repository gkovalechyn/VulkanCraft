#include "ResourceManager.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

VulkanCraft::Graphics::ResourceManager::ResourceManager(vk::PhysicalDevice& physicalDevice, vk::Device & device, vk::Queue transferQueue, uint32_t queueFamilyIndex) : device(device), transferQueue(transferQueue) {
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.physicalDevice = physicalDevice;

	auto result = vmaCreateAllocator(&allocatorCreateInfo, &(this->vma));

	vk::CommandPoolCreateInfo createInfo;
	createInfo
		.setQueueFamilyIndex(queueFamilyIndex);

	this->commandPool = device.createCommandPool(createInfo);

	VkBuffer vertexBufferHandle;
	auto vertexAllocation = this->allocateVertexBuffer(64 * 1024 * 1024, &vertexBufferHandle);
	this->vertexBufferManager = std::make_unique<BufferMemoryManager>(vk::Buffer(vertexBufferHandle), vertexAllocation, 64 * 1024 * 1024);

	VkBuffer indexBufferHandle;
	auto indexAllocation = this->allocateVertexBuffer(64 * 1024 * 1024, &indexBufferHandle);
	this->vertexBufferManager = std::make_unique<BufferMemoryManager>(vk::Buffer(indexBufferHandle), indexAllocation, 64 * 1024 * 1024);

	VkBuffer uboBufferHandle;
	auto uboAllocation = this->allocateBuffer(128 * 1024 * 1024, &uboBufferHandle, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY);
	this->vertexBufferManager = std::make_unique<BufferMemoryManager>(vk::Buffer(uboBufferHandle), uboAllocation, 128 * 1024 * 1024);

	VkBuffer stagingBufferHandle;
	auto stagingAllocation = this->allocateStagingBuffer(64 * 1024 * 1024, &stagingBufferHandle);
	this->vertexBufferManager = std::make_unique<BufferMemoryManager>(vk::Buffer(stagingBufferHandle), stagingAllocation, 64 * 1024 * 1024);

	this->mapAllocation(this->stagingBufferManager->getBufferAllocation(), &this->mappedStagingBuffer);
}

VulkanCraft::Graphics::ResourceManager::~ResourceManager() {
	vmaDestroyAllocator(this->vma);
}

VmaAllocation VulkanCraft::Graphics::ResourceManager::allocateVertexBuffer(uint64_t sizeInBytes, VkBuffer* outHandle) {
	return this->allocateBuffer(sizeInBytes, outHandle, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY);
}

VmaAllocation VulkanCraft::Graphics::ResourceManager::allocateIndexBuffer(uint64_t sizeInBytes, VkBuffer* outHandle) {
	return this->allocateBuffer(sizeInBytes, outHandle, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY);
}

VmaAllocation VulkanCraft::Graphics::ResourceManager::allocateStagingBuffer(uint64_t sizeInBytes, VkBuffer * outHandle) {
	return this->allocateBuffer(sizeInBytes, outHandle, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst, VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU);
}

VmaAllocation VulkanCraft::Graphics::ResourceManager::allocateBuffer(uint64_t sizeInBytes, VkBuffer * outHandle, vk::BufferUsageFlags usageFlags, VmaMemoryUsage usage) {
	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo
		.setSize(sizeInBytes)
		.setUsage(usageFlags);

	VmaAllocationCreateInfo allocationCreateInfo;
	allocationCreateInfo.usage = usage;

	VmaAllocation allocation;
	VkBufferCreateInfo proxy = bufferCreateInfo;

	vmaCreateBuffer(this->vma, &proxy, &allocationCreateInfo, outHandle, &allocation, nullptr);

	return allocation;
}

void VulkanCraft::Graphics::ResourceManager::queueMemoryTransfer(const vk::Buffer & from, const vk::Buffer & to, const uint64_t size, const uint64_t offsetFrom, const uint64_t offsetTo) {

}

void VulkanCraft::Graphics::ResourceManager::queueFrameImportantMemoryTransfer(const vk::Buffer & from, const vk::Buffer & to, const uint64_t size, const uint64_t offsetFrom, const uint64_t offsetTo) {
}

void VulkanCraft::Graphics::ResourceManager::mapAllocation(const VmaAllocation & allocation, void ** ptr) {
	vmaMapMemory(this->vma, allocation, ptr);
}

void VulkanCraft::Graphics::ResourceManager::unmapAllocation(const VmaAllocation & allocation) {
	vmaUnmapMemory(this->vma, allocation);
}

std::future<void> VulkanCraft::Graphics::ResourceManager::pushDataToGPUBuffer(const void * data, const uint64_t size, const GPUAllocation& to, const bool important) {
	GPUAllocation stagingAllocation = this->stagingBufferManager->allocateMemory(size, 1);
	VmaAllocation vmaAllocation = this->stagingBufferManager->getBufferAllocation();
	
	memcpy(((uint8_t*) this->mappedStagingBuffer) + stagingAllocation.offset, data, size);

	vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo
		.setCommandBufferCount(1)
		.setCommandPool(this->commandPool)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	PendingMemoryTransfer transfer = {};
	transfer.freeFromBuffer = false;
	transfer.from = this->stagingBufferManager->getManagedBuffer();
	transfer.fromAllocation = vmaAllocation;
	transfer.fromOffset = stagingAllocation.offset;

	transfer.to = to.buffer;
	transfer.toOffset = to.offset;

	transfer.doneFence = this->device.createFence({});
	transfer.doneSemaphore = this->device.createSemaphore({});
	transfer.commandBuffer = this->device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	transfer.commandBuffer.begin(beginInfo);

	vk::BufferCopy copyRegion;
	copyRegion
		.setSize(size)
		.setSrcOffset(stagingAllocation.offset)
		.setDstOffset(to.offset);

	transfer.commandBuffer.copyBuffer(transfer.from, transfer.to, { copyRegion });
	transfer.commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo
		.setCommandBufferCount(1)
		.setPCommandBuffers(&transfer.commandBuffer)
		.setSignalSemaphoreCount(1)
		.setPWaitSemaphores(&transfer.doneSemaphore);

	this->transferQueue.submit(submitInfo, transfer.doneFence);
	auto future = transfer.promise.get_future();

	if (important) {
		this->pendingImportantTransfers.push_back(std::move(transfer));
	} else {
		this->pendingTransfers.push_back(std::move(transfer));
	}

	return future;
}

void VulkanCraft::Graphics::ResourceManager::uploadMeshToGPU(Mesh & mesh) {

	GPUAllocation vertexAllocation = this->vertexBufferManager->allocateMemory(sizeof(Vertex) * mesh.getVertexCount(), 16);
	GPUAllocation indexAllocation = this->indexBufferManager->allocateMemory(sizeof(uint32_t) * mesh.getVertexCount(), sizeof(uint32_t));

	mesh.setVertexBuffer(vertexAllocation);
	mesh.setIndexBuffer(indexAllocation);

	this->pushDataToGPUBuffer(mesh.getVertices().data(), mesh.getVertices().size() * sizeof(Vertex), vertexAllocation, true);
	this->pushDataToGPUBuffer(mesh.getIndices().data(), mesh.getIndices().size() * sizeof(uint32_t), indexAllocation, true);
}

void VulkanCraft::Graphics::ResourceManager::refreshMeshGPUData(Mesh & mesh) {
	this->pushDataToGPUBuffer(mesh.getVertices().data(), mesh.getVertices().size() * sizeof(Vertex), mesh.getVertexBuffer(), true);
	this->pushDataToGPUBuffer(mesh.getIndices().data(), mesh.getIndices().size() * sizeof(uint32_t), mesh.getIndexBuffer(), true);
}

const std::vector<PendingMemoryTransfer>& VulkanCraft::Graphics::ResourceManager::getImportantPendingTransfers() const noexcept {
	return this->pendingImportantTransfers;
}

const std::vector<PendingMemoryTransfer>& VulkanCraft::Graphics::ResourceManager::getPendingTransfers() const noexcept {
	return this->pendingTransfers;
}

void VulkanCraft::Graphics::ResourceManager::updateTransfers() noexcept {
	//TODO find a way that doesn't require copying the loop for both vectors
	for (auto it = this->pendingImportantTransfers.rbegin(); it != this->pendingImportantTransfers.rend(); it++) {
		auto& pendingTransfer = *it;
		auto fenceStatus = this->device.getFenceStatus(pendingTransfer.doneFence);

		if (fenceStatus == vk::Result::eSuccess) {
			pendingTransfer.promise.set_value();
			this->pendingImportantTransfers.erase(it.base());
		}
	}

	for (auto it = this->pendingTransfers.rbegin(); it != this->pendingTransfers.rend(); it++) {
		auto& pendingTransfer = *it;
		auto fenceStatus = this->device.getFenceStatus(pendingTransfer.doneFence);

		if (fenceStatus == vk::Result::eSuccess) {
			pendingTransfer.promise.set_value();
			this->pendingTransfers.erase(it.base());
		}
	}
}