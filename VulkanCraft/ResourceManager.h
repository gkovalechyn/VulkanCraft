#pragma once
#include <vulkan/vulkan.hpp>
#include "libs/vk_mem_alloc.h"

namespace VulkanCraft {
	namespace Graphics {
		class ResourceManager {
		public:
			ResourceManager(const vk::PhysicalDevice& physicalDevice, const vk::Device & device, vk::Queue transferQueue);
			~ResourceManager();

			VmaAllocation allocateVertexBuffer(uint64_t sizeInBytes);
		private:
			vk::Device device;
			vk::Queue transferQueue;

			VmaAllocator vma;
		};
	}
}

