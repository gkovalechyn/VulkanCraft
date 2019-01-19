#pragma once
#include <algorithm>
#include "GPUAllocation.h"
#include "libs/vk_mem_alloc.h"

namespace VulkanCraft {
	namespace Graphics {
		class BufferMemoryManager {
		public:
			BufferMemoryManager(vk::Buffer buffer, VmaAllocation allocation, uint64_t size);
			~BufferMemoryManager();

			GPUAllocation allocateMemory(const uint64_t size, const uint32_t requiredAlignment);
			void freeAllocation(const GPUAllocation& allocation);

			uint64_t getFreeMemorySize();
			uint64_t getAllocatedMemorySize();

			uint32_t getCurrentAllocationCount();
			uint32_t getFreeBlockCount();

			vk::Buffer getManagedBuffer();
			VmaAllocation getBufferAllocation();

		private:
			struct MemoryBlock {
				MemoryBlock(uint64_t base, uint64_t size) : allocationID{ 0 }, base{ base }, size{ size } {};
				MemoryBlock(uint64_t allocationId, uint64_t base, uint64_t size) : allocationID{ allocationId }, base{ base }, size{ size } {};

				uint64_t allocationID;
				uint64_t base;
				uint64_t size = 0;
			};

			const vk::Buffer buffer;
			const VmaAllocation allocation;
			
			uint64_t nextAllocationID = 1;
			std::vector<MemoryBlock> usedBlocks;
			std::vector<MemoryBlock> freeBlocks;
		};
	}
}

