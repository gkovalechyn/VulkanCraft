#include "ResourceManager.h"


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

