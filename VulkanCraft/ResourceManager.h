#pragma once
#include <vulkan/vulkan.hpp>
#include "Mesh.h"

namespace VulkanCraft {
	namespace Graphics {
		struct PendingMemoryTransfer {
			vk::Semaphore done;
			vk::CommandBuffer commandBuffer;
			vk::Buffer from;
			vk::Buffer to;
			uint64_t offset;
			uint64_t size;
		};

		class ResourceManager {
		public:
			ResourceManager(vk::PhysicalDevice& physicalDevice, vk::Device & device, vk::Queue transferQueue, uint32_t queueFamilyIndex);
			~ResourceManager();

			VmaAllocation allocateVertexBuffer(uint64_t sizeInBytes, VkBuffer * outHandle);
			VmaAllocation allocateIndexBuffer(uint64_t sizeInBytes, VkBuffer * outHandle);
			VmaAllocation allocateStagingBuffer(uint64_t sizeInBytes, VkBuffer * outHandle);

			VmaAllocation allocateBuffer(uint64_t sizeInBytes, VkBuffer * outHandle, vk::BufferUsageFlags usageFlags, VmaMemoryUsage usage);

			void allocateBuffersForMesh(const Core::Mesh& mesh, vk::Buffer* vertex, vk::Buffer* index);
		private:
			vk::Device device;
			vk::Queue transferQueue;
			vk::CommandPool commandPool;
			std::vector<PendingMemoryTransfer> pendingTransfers;

			VmaAllocator vma;
		};
	}
}

