#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		class Renderable {
		public:
			virtual ~Renderable() {};

			virtual void writeRenderCommands(const vk::CommandBuffer& commandBuffer) = 0;
		};
	}
}

