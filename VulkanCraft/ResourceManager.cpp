#include "ResourceManager.h"


VulkanCraft::Graphics::ResourceManager::ResourceManager(const vk::PhysicalDevice& physicalDevice, const vk::Device & device, vk::Queue transferQueue) : device(device), transferQueue(transferQueue){
	VmaAllocatorCreateInfo allocatorCreateInfo;
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.physicalDevice = physicalDevice;

	vmaCreateAllocator(&allocatorCreateInfo, &this->vma);

	vk::CommandPoolCreateInfo commandPoolCreateInfo;
}

VulkanCraft::Graphics::ResourceManager::~ResourceManager() {
	vmaDestroyAllocator(this->vma);
}

VmaAllocation VulkanCraft::Graphics::ResourceManager::allocateVertexBuffer(uint64_t sizeInBytes) {
	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo
		.setSize(sizeInBytes)
		.setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst);

	VmaAllocationCreateInfo allocationCreateInfo;
	allocationCreateInfo.flags = VMA_MEMORY_USAGE_GPU_ONLY;

	VkBuffer buffer;
	VmaAllocation allocation;
	VkBufferCreateInfo proxy = bufferCreateInfo;

	vmaCreateBuffer(this->vma, &proxy, &allocationCreateInfo, &buffer, &allocation, nullptr);
}


