#include "ResourceManager.h"

using namespace VulkanCraft;

VulkanCraft::Graphics::ResourceManager::ResourceManager(vk::PhysicalDevice& physicalDevice, vk::Device & device, vk::Queue transferQueue, uint32_t queueFamilyIndex) : device(device), transferQueue(transferQueue){
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.physicalDevice = physicalDevice;

	auto result = vmaCreateAllocator(&allocatorCreateInfo, &(this->vma));

	vk::CommandPoolCreateInfo createInfo;
	createInfo
		.setQueueFamilyIndex(queueFamilyIndex);

	this->commandPool = device.createCommandPool(createInfo);

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

std::future<void> VulkanCraft::Graphics::ResourceManager::pushDataToGPUBuffer(const void * data, const uint64_t size, const vk::Buffer & to, const uint64_t offset, const bool important) {
	VkBuffer stagingBufferHandle;
	VmaAllocation allocation = this->allocateStagingBuffer(size, &stagingBufferHandle);
	vk::Buffer stagingBuffer(stagingBufferHandle);

	vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo
		.setCommandBufferCount(1)
		.setCommandPool(this->commandPool)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	PendingMemoryTransfer transfer = {};
	transfer.promise = std::make_unique<std::promise<void>>();
	transfer.freeFromBuffer = true;
	transfer.from = stagingBuffer;
	transfer.fromAllocation = allocation;

	transfer.to = to;
	transfer.toOffset = offset;

	transfer.doneSemaphore = this->device.createSemaphore({});
	transfer.commandBuffer = this->device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	transfer.commandBuffer.begin(beginInfo);

	vk::BufferCopy copyRegion;
	copyRegion
		.setSize(size)
		.setSrcOffset(0)
		.setDstOffset(offset);
	transfer.commandBuffer.copyBuffer(transfer.from, transfer.to, { copyRegion });
	transfer.commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo
		.setCommandBufferCount(1)
		.setPCommandBuffers(&transfer.commandBuffer)
		.setSignalSemaphoreCount(1)
		.setPSignalSemaphores(&transfer.doneSemaphore);


	this->transferQueue.submit(submitInfo, nullptr);

	if (important) {
		this->pendingImportantTransfers.push_back(transfer);
	} else {
		this->pendingTransfers.push_back(transfer);
	}

	return transfer.promise->get_future();
}

Core::Mesh* VulkanCraft::Graphics::ResourceManager::createMesh(const tinyobj::attrib_t & attributes, const tinyobj::shape_t & shape) {
	Core::Mesh* mesh = new Core::Mesh(attributes, shape);
	VkBuffer vertexBufferHandle;
	VkBuffer indexBufferHandle;

	VmaAllocation vertexAllocation = this->allocateVertexBuffer(mesh->getVertexCount() * sizeof(Vertex), &vertexBufferHandle);
	VmaAllocation indexAllocation = this->allocateIndexBuffer(mesh->getIndexCount() * sizeof(uint32_t), &indexBufferHandle);

	vk::Buffer vertexBuffer(vertexBufferHandle);
	vk::Buffer indexBuffer(indexBufferHandle);

	mesh->setVertexBuffer(vertexAllocation, vertexBuffer);
	mesh->setIndexBuffer(indexAllocation, indexBuffer);
	
	return mesh;
}

/*
void VulkanCraft::Graphics::ResourceManager::allocateBuffersForMesh(const Core::Mesh & mesh, vk::Buffer * vertex, vk::Buffer * index) {
	uint64_t vertexBufferSize = sizeof(Vertex) * mesh.getVertexCount();
	uint64_t indexBufferSize = sizeof(uint32_t) * mesh.getIndexCount();
	VkBuffer tempVertex;
	VkBuffer tempIndex;

	VmaAllocation vertexAllocation = this->allocateVertexBuffer(vertexBufferSize, &tempVertex);
	VmaAllocation indexAllocation = this->allocateIndexBuffer(vertexBufferSize, &tempIndex);

	*vertex = tempVertex;
	*index = tempIndex;
}
*/