#include "ResourceManager.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

VulkanCraft::Graphics::ResourceManager::ResourceManager(const GraphicalDevice& device, vk::DescriptorSetLayout pipelineLayout) : device{ &device }, layout { pipelineLayout } {
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.device = (VkDevice) device.logicalDevice;
	allocatorCreateInfo.physicalDevice = (VkPhysicalDevice) device.physicalDevice;

	auto result = vmaCreateAllocator(&allocatorCreateInfo, &(this->vma));

	vk::CommandPoolCreateInfo createInfo;
	createInfo
		.setQueueFamilyIndex(device.graphicsFamilyIndex);

	this->commandPool = device.logicalDevice.createCommandPool(createInfo);

	VkBuffer vertexBufferHandle;
	auto vertexAllocation = this->allocateVertexBuffer(64 * 1024, &vertexBufferHandle);
	VmaAllocationInfo info;
	vmaGetAllocationInfo(this->vma, vertexAllocation, &info);
	this->vertexBufferManager = std::make_unique<BufferMemoryManager>(vk::Buffer(vertexBufferHandle), vertexAllocation, info);

	VkBuffer indexBufferHandle;
	auto indexAllocation = this->allocateIndexBuffer(64 * 1024, &indexBufferHandle);
	vmaGetAllocationInfo(this->vma, indexAllocation, &info);
	this->indexBufferManager = std::make_unique<BufferMemoryManager>(vk::Buffer(indexBufferHandle), indexAllocation, info);

	VkBuffer uboBufferHandle;
	auto uboAllocation = this->allocateBuffer(128 * 1024, &uboBufferHandle, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY);
	vmaGetAllocationInfo(this->vma, uboAllocation, &info);
	this->uboBufferManager = std::make_unique<BufferMemoryManager>(vk::Buffer(uboBufferHandle), uboAllocation, info);

	VkBuffer stagingBufferHandle;
	auto stagingAllocation = this->allocateStagingBuffer(64 * 1024, &stagingBufferHandle);
	vmaGetAllocationInfo(this->vma, stagingAllocation, &info);
	this->stagingBufferManager = std::make_unique<BufferMemoryManager>(vk::Buffer(stagingBufferHandle), stagingAllocation, info);

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
	vk::BufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo
		.setSize(sizeInBytes)
		.setUsage(usageFlags);

	VmaAllocationCreateInfo allocationCreateInfo = {};
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
	GPUAllocation stagingAllocation = this->stagingBufferManager->allocateMemory(size);
	//VmaAllocation vmaAllocation = this->stagingBufferManager->getBufferAllocation();
	
	Core::Logger::vaLog(Core::LogLevel::eDebug, "Allocated block from staging buffer, id: %d, offset: %d, size: %d", stagingAllocation.id, stagingAllocation.offset, stagingAllocation.size);
	memcpy(((uint8_t*) this->mappedStagingBuffer) + stagingAllocation.offset, data, size);

	vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo
		.setCommandBufferCount(1)
		.setCommandPool(this->commandPool)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	PendingMemoryTransfer transfer = {};
	transfer.freeFromBuffer = false;
	transfer.from = this->stagingBufferManager->getManagedBuffer();
	//transfer.fromAllocation = vmaAllocation;
	transfer.fromOffset = stagingAllocation.offset;

	transfer.to = to.buffer;
	transfer.toOffset = to.offset;
	transfer.size = size;

	transfer.doneFence = this->device->logicalDevice.createFence({});
	transfer.doneSemaphore = this->device->logicalDevice.createSemaphore({});
	transfer.commandBuffer = this->device->logicalDevice.allocateCommandBuffers(commandBufferAllocateInfo)[0];

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	transfer.commandBuffer.begin(beginInfo);

	vk::BufferCopy copyRegion;
	copyRegion
		.setSize(size)
		.setSrcOffset(stagingAllocation.offset)
		.setDstOffset(to.offset);

	Core::Logger::vaLog(Core::LogLevel::eDebug, "Copy %d byte region from %d to %d", copyRegion.size, copyRegion.srcOffset, copyRegion.dstOffset);

	transfer.commandBuffer.copyBuffer(transfer.from, transfer.to, { copyRegion });
	transfer.commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo
		.setCommandBufferCount(1)
		.setPCommandBuffers(&transfer.commandBuffer)
		.setSignalSemaphoreCount(1)
		.setPSignalSemaphores(&transfer.doneSemaphore);

	Core::Logger::debug("Submitting copy command to the queue");

	this->device->graphicsQueue.submit(submitInfo, transfer.doneFence);
	auto future = transfer.promise.get_future();

	if (important) {
		Core::Logger::debug("Pushing copy to the important transfer queue");
		this->pendingImportantTransfers.push_back(std::move(transfer));
	} else {
		Core::Logger::debug("Pushing copy to the normal transfer queue");
		this->pendingTransfers.push_back(std::move(transfer));
	}

	return future;
}

void VulkanCraft::Graphics::ResourceManager::uploadMeshToGPU(Mesh & mesh) {
	GPUAllocation vertexAllocation = this->vertexBufferManager->allocateMemory(sizeof(Vertex) * mesh.getVertexCount());
	Core::Logger::vaLog(Core::LogLevel::eDebug, "Allocated vertex buffer %d at %d with size %d", vertexAllocation.id, vertexAllocation.offset, vertexAllocation.size);
	GPUAllocation indexAllocation = this->indexBufferManager->allocateMemory(sizeof(uint32_t) * mesh.getIndexCount());
	Core::Logger::vaLog(Core::LogLevel::eDebug, "Allocated index buffer %d at %d with size %d", indexAllocation.id, indexAllocation.offset, indexAllocation.size);

	mesh.setVertexBuffer(vertexAllocation);
	mesh.setIndexBuffer(indexAllocation);

	Core::Logger::debug("Pushing vertex buffer to the GPU");
	this->pushDataToGPUBuffer(mesh.getVertices().data(), mesh.getVertices().size() * sizeof(Vertex), vertexAllocation, true);
	Core::Logger::debug("Pushing index buffer to the GPU");
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

	std::vector<PendingMemoryTransfer> newImportantList;
	for (auto it = this->pendingImportantTransfers.rbegin(); it != this->pendingImportantTransfers.rend(); it++) {
		auto& pendingTransfer = *it;
		auto fenceStatus = this->device->logicalDevice.getFenceStatus(pendingTransfer.doneFence);

		if (fenceStatus == vk::Result::eSuccess) {
			pendingTransfer.promise.set_value();

			Core::Logger::vaLog(
				Core::LogLevel::eDebug,
				"Important memory transfer of %d bytes finished: from %d:%d to %d:%d",
				pendingTransfer.size,
				pendingTransfer.from,
				pendingTransfer.fromOffset,
				pendingTransfer.to,
				pendingTransfer.toOffset
			);

			this->device->logicalDevice.destroyFence(pendingTransfer.doneFence);
			this->device->logicalDevice.destroySemaphore(pendingTransfer.doneSemaphore);
			this->device->logicalDevice.freeCommandBuffers(this->commandPool, { pendingTransfer.commandBuffer });

			//this->pendingImportantTransfers.erase(std::next(it).base());
		} else {
			newImportantList.push_back(std::move(pendingTransfer));
		}
	}

	this->pendingImportantTransfers = std::move(newImportantList);

	std::vector<PendingMemoryTransfer> newPendingList;
	for (auto it = this->pendingTransfers.rbegin(); it != this->pendingTransfers.rend(); it++) {
		auto& pendingTransfer = *it;
		auto fenceStatus = this->device->logicalDevice.getFenceStatus(pendingTransfer.doneFence);

		if (fenceStatus == vk::Result::eSuccess) {
			pendingTransfer.promise.set_value();

			Core::Logger::vaLog(
				Core::LogLevel::eDebug,
				"Normal memory transfer of %d bytes finished: from %d:%d to %d:%d",
				pendingTransfer.size,
				pendingTransfer.from,
				pendingTransfer.fromOffset,
				pendingTransfer.to,
				pendingTransfer.toOffset
			);

			//TODO Free allocated resources

			//std::advance(it, 1);
			//this->pendingTransfers.erase(std::next(it).base());
		} else {
			newPendingList.push_back(std::move(pendingTransfer));
		}
	}

	this->pendingTransfers = std::move(newPendingList);
}

void VulkanCraft::Graphics::ResourceManager::createModelUbo(const uint32_t maxModelCount) {
	VkBuffer handle;

	//https://github.com/SaschaWillems/Vulkan/tree/master/examples/dynamicuniformbuffer
	auto minUboAlignment = this->device->properties.limits.minUniformBufferOffsetAlignment;
	auto dynamicAlignment = sizeof(glm::mat4);
	if (minUboAlignment > 0) {
		dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}

	this->modelUboAlignment = dynamicAlignment;

	this->modelUboAllocation = this->allocateBuffer(
		maxModelCount * dynamicAlignment,
		&handle,
		vk::BufferUsageFlagBits::eUniformBuffer,
		VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU
	);

	this->modelUboBuffer = vk::Buffer(handle);

	vk::DescriptorPoolSize poolSize = {};
	poolSize
		.setDescriptorCount(1)
		.setType(vk::DescriptorType::eStorageBufferDynamic);

	vk::DescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
		.setPoolSizeCount(1)
		.setPPoolSizes(&poolSize)
		.setMaxSets(10);

	this->modelDescriptorPool = this->device->logicalDevice.createDescriptorPool(poolCreateInfo);

	vk::DescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo
		.setDescriptorPool(this->modelDescriptorPool)
		.setDescriptorSetCount(1)
		.setPSetLayouts(&this->layout);

	this->modelDescriptorSet = this->device->logicalDevice.allocateDescriptorSets(allocateInfo)[0];


	vk::DescriptorBufferInfo bufferInfo = {};
	bufferInfo
		.setBuffer(this->modelUboBuffer)
		.setOffset(0)
		.setRange(VK_WHOLE_SIZE);

	vk::WriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
		.setDstBinding(0)
		.setDstSet(this->modelDescriptorSet)
		.setPBufferInfo(&bufferInfo);

	this->device->logicalDevice.updateDescriptorSets({ writeDescriptorSet }, {});
}

void VulkanCraft::Graphics::ResourceManager::mapModelDynamicUbo(void ** ptr) {
	vmaMapMemory(this->vma, this->modelUboAllocation, ptr);
}

void VulkanCraft::Graphics::ResourceManager::unmapModelDynamicUbo() {
	vmaUnmapMemory(this->vma, this->modelUboAllocation);
}

uint64_t VulkanCraft::Graphics::ResourceManager::getModelUboRequiredAlignment() {
	return this->modelUboAlignment;
}

vk::DescriptorSet VulkanCraft::Graphics::ResourceManager::getModelDescriptorSet() {
	return this->modelDescriptorSet;
}

void VulkanCraft::Graphics::ResourceManager::flushUBOBuffer() {
	vmaFlushAllocation(this->vma, this->modelUboAllocation, 0, VK_WHOLE_SIZE);
}	
