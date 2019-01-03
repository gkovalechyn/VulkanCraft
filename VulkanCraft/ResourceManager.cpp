#include "ResourceManager.h"


VulkanCraft::Graphics::ResourceManager::ResourceManager(const vk::PhysicalDevice& physicalDevice, const vk::Device & device, vk::Queue transferQueue) : device(device), transferQueue(transferQueue){
	VmaAllocatorCreateInfo allocatorCreateInfo;
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.physicalDevice = physicalDevice;

	vmaCreateAllocator(&allocatorCreateInfo, &this->vma);
}

VulkanCraft::Graphics::ResourceManager::~ResourceManager() {
	vmaDestroyAllocator(this->vma);
}

VmaAllocation VulkanCraft::Graphics::ResourceManager::allocateVertexBuffer(uint64_t sizeInBytes) {
	
}

