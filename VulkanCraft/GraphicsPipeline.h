#pragma once
#include <vulkan/vulkan.hpp>
namespace VulkanCraft {
	namespace Graphics {
		class GraphicsPipeline {

		public:
			GraphicsPipeline();
			virtual ~GraphicsPipeline();
	
			virtual vk::Pipeline getHandle() = 0;
			virtual vk::RenderPass getRenderPass() = 0;
			virtual vk::Rect2D getRenderArea() = 0;

			virtual void recreate() = 0;
			virtual void cleanup() = 0;

		};
	}
}
