#pragma once
#include <future>
#include <vulkan/vulkan.hpp>
#include "Mesh.h"
#include "BufferMemoryManager.h"

namespace VulkanCraft {
	namespace Graphics {
		struct PendingMemoryTransfer {
			vk::Fence doneFence;
			vk::Semaphore doneSemaphore;
			vk::CommandBuffer commandBuffer;

			VmaAllocation fromAllocation;
			vk::Buffer from;
			uint64_t fromOffset = 0;

			VmaAllocation toAllocation;
			vk::Buffer to;
			uint64_t toOffset = 0;
			
			uint64_t size = 0;

			bool freeFromBuffer = false;
			std::promise<void> promise;
		};

		class ResourceManager {
		public:
			ResourceManager(vk::PhysicalDevice& physicalDevice, vk::Device & device, vk::Queue transferQueue, uint32_t queueFamilyIndex);
			~ResourceManager();

			VmaAllocation allocateVertexBuffer(uint64_t sizeInBytes, VkBuffer * outHandle);
			VmaAllocation allocateIndexBuffer(uint64_t sizeInBytes, VkBuffer * outHandle);
			VmaAllocation allocateStagingBuffer(uint64_t sizeInBytes, VkBuffer * outHandle);

			VmaAllocation allocateBuffer(uint64_t sizeInBytes, VkBuffer * outHandle, vk::BufferUsageFlags usageFlags, VmaMemoryUsage usage);

			void queueMemoryTransfer(const vk::Buffer& from, const vk::Buffer& to, const uint64_t size, const uint64_t offsetFrom = 0, const uint64_t offsetTo = 0);
			void queueFrameImportantMemoryTransfer(const vk::Buffer& from, const vk::Buffer& to, const uint64_t size, const uint64_t offsetFrom = 0, const uint64_t offsetTo = 0);

			void mapAllocation(const VmaAllocation& allocation, void** ptr);
			void unmapAllocation(const VmaAllocation& allocation);

			std::future<void> pushDataToGPUBuffer(const void* data, const uint64_t size, const GPUAllocation& allocation, const bool important = false);
			void uploadMeshToGPU(Mesh& mesh);
			void refreshMeshGPUData(Mesh& mesh);

			const std::vector<PendingMemoryTransfer>& getImportantPendingTransfers() const noexcept;
			const std::vector<PendingMemoryTransfer>& getPendingTransfers() const noexcept;

			void updateTransfers() noexcept;
		private:
			vk::Device device;
			vk::Queue transferQueue;
			vk::CommandPool commandPool;

			std::vector<PendingMemoryTransfer> pendingTransfers;
			std::vector<PendingMemoryTransfer> pendingImportantTransfers;

			std::unique_ptr<BufferMemoryManager> vertexBufferManager;
			std::unique_ptr<BufferMemoryManager> indexBufferManager;
			std::unique_ptr<BufferMemoryManager> stagingBufferManager;
			std::unique_ptr<BufferMemoryManager> uboBufferManager;

			void* mappedStagingBuffer;

			VmaAllocator vma;
		};
	}
}

