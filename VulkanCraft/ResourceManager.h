#pragma once
#include <vulkan/vulkan.hpp>
#include "Mesh.h"

namespace VulkanCraft {
	namespace Graphics {
		class ResourceManager {
		public:
			ResourceManager(vk::PhysicalDevice& physicalDevice, vk::Device & device, vk::Queue transferQueue);
			~ResourceManager();

			VmaAllocation allocateVertexBuffer(uint64_t sizeInBytes, VkBuffer * outHandle);

			VmaAllocation allocateIndexBuffer(uint64_t sizeInBytes, VkBuffer * outHandle);

			Core::Mesh* loadMesh(std::string path);
		private:
			vk::Device device;
			vk::Queue transferQueue;
			vk::CommandPool commandPool;

			VmaAllocator vma;
		};
	}
}

