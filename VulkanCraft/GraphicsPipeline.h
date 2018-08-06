#pragma once
#include <vulkan/vulkan.hpp>
namespace VulkanCraft {
	namespace Graphics {
		class GraphicsPipeline {

		public:
			GraphicsPipeline();
			~GraphicsPipeline();
	
			virtual vk::Pipeline getHandle() = 0;
			virtual void recreate() = 0;
			virtual void cleanup() = 0;
		};
	}
}
