#include "BufferMemoryManager.h"

using namespace VulkanCraft::Graphics;

VulkanCraft::Graphics::BufferMemoryManager::BufferMemoryManager(vk::Buffer buffer, VmaAllocation allocation, VmaAllocationInfo info, uint32_t blockSize) : buffer{ buffer }, allocation{ allocation }, blockSize{ blockSize } {
	auto initialFreeBlock = MemoryBlock(0, static_cast<uint64_t>(0), info.size);

	this->freeBlocks.push_back(initialFreeBlock);
}

VulkanCraft::Graphics::BufferMemoryManager::~BufferMemoryManager() {
}

GPUAllocation VulkanCraft::Graphics::BufferMemoryManager::allocateMemory(const uint64_t size) {
	uint64_t realSize;

	if (this->blockSize > 0) {
		int numBlocks = static_cast<int>(std::ceil(size / (float)this->blockSize));
		realSize = numBlocks * this->blockSize;
	} else {
		realSize = size;
	}

	for (auto it = this->freeBlocks.begin(); it != this->freeBlocks.end(); it++) {
		auto freeBlock = *it;

		if (freeBlock.size >= realSize) {
			auto remainingBlock = MemoryBlock(freeBlock.base + realSize, freeBlock.size - realSize);
			
			if (remainingBlock.size > 0) {
				//To prevent moving all the remaining elements twice, once for the deletion and another one for the insertion
				*it = remainingBlock;
			} else {
				this->freeBlocks.erase(it);
			}

			auto allocation = GPUAllocation(
				this->nextAllocationID++,
				this->buffer,
				realSize,
				freeBlock.base
			);

			freeBlock.size -= remainingBlock.size;
			freeBlock.allocationID = allocation.id;

			this->usedBlocks.push_back(freeBlock);
			return allocation;
		}
	}

	throw std::runtime_error("Not enough free memory");
}

void VulkanCraft::Graphics::BufferMemoryManager::freeAllocation(const GPUAllocation & allocation) {
	for (auto it = this->usedBlocks.begin(); it != this->usedBlocks.end(); it++) {
		const auto& usedBlock = *it;

		const auto allocationEnd = allocation.offset + allocation.size;
		bool foundBlock = false;

		if (usedBlock.allocationID == allocation.id) {
			for (auto freeIt = this->freeBlocks.begin(); freeIt != this->freeBlocks.end(); freeIt++) {
				auto& freeBlock = *freeIt;

				//The block we're trying to free is exactly the one before this one
				if (allocationEnd == freeBlock.base) {
					freeBlock.base -= allocation.size;
					this->usedBlocks.erase(it);
					foundBlock = true;
				} else if (allocation.offset == (freeBlock.base + freeBlock.size)) { //It is the one after the free block
					freeBlock.base += allocation.size;
					this->usedBlocks.erase(it);
					foundBlock = true;
				}
			}

			//Freeing an isolated allocation
			if (!foundBlock) {
				bool foundPlaceToInsert = false;
				auto blockToInsert = MemoryBlock(allocation.offset, allocation.size);

				for (auto freeIt = this->freeBlocks.begin(); freeIt != this->freeBlocks.end() - 1; freeIt++) {
					auto& currentBlock = *freeIt;
					auto& nextBlock = *(freeIt + 1);

					//Is between these 2 free blocks
					if (blockToInsert.base > (currentBlock.base + currentBlock.size) && blockToInsert.base < nextBlock.base) {
						foundPlaceToInsert = true;
						this->freeBlocks.insert(freeIt, blockToInsert);
						break;
					}

				}
				//If still haven't found a place to insert it, must be the last block or the first block
				if (!foundPlaceToInsert) {
					if (this->freeBlocks.size() > 0) {
						if (blockToInsert.base > this->freeBlocks[0].base) {//Last block
							this->freeBlocks.push_back(blockToInsert);
						} else {//First block
							this->freeBlocks.insert(this->freeBlocks.begin(), blockToInsert);
						}
					} else {
						this->freeBlocks.push_back(blockToInsert);
					}
				}
			}
		}
	}

	throw std::runtime_error("Tried to free allocation that did not belong to this buffer");
}

uint64_t VulkanCraft::Graphics::BufferMemoryManager::getFreeMemorySize() {
	uint64_t totalSize = 0;

	for (const auto& block : this->freeBlocks) {
		totalSize += block.size;
	}

	return totalSize;
}

uint64_t VulkanCraft::Graphics::BufferMemoryManager::getAllocatedMemorySize() {
	uint64_t totalSize = 0;

	for (const auto& block : this->usedBlocks) {
		totalSize += block.size;
	}

	return totalSize;
}

uint32_t VulkanCraft::Graphics::BufferMemoryManager::getCurrentAllocationCount() {
	return static_cast<uint32_t>(this->usedBlocks.size());
}

uint32_t VulkanCraft::Graphics::BufferMemoryManager::getFreeBlockCount() {
	return static_cast<uint32_t>(this->freeBlocks.size());
}

vk::Buffer VulkanCraft::Graphics::BufferMemoryManager::getManagedBuffer() {
	return this->buffer;
}

VmaAllocation VulkanCraft::Graphics::BufferMemoryManager::getBufferAllocation() {
	return this->allocation;
}
