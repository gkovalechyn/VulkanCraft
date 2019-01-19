#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		struct GPUAllocation {
			GPUAllocation() : id{ 0 }, buffer { nullptr }, size{ 0 }, offset{ 0 } {};
			GPUAllocation(uint64_t id, vk::Buffer buffer, uint64_t size, uint64_t offset) :
				id{ id },
				buffer{ buffer },
				size{ size },
				offset{ offset }
			{};

			const uint64_t id;
			const vk::Buffer buffer;
			const uint64_t size;
			const uint64_t offset;
		};
	}
}
