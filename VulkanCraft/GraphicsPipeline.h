#pragma once
#include <vulkan/vulkan.hpp>
#include "GPUAllocation.h"

namespace VulkanCraft {
	namespace Graphics {
		struct DescriptorSetData {
		public:
			vk::DescriptorSet descriptorSet;
			vk::DescriptorType type;
			uint32_t set;
			uint32_t binding;
			GPUAllocation data;

			bool dirty;
		};

		class GraphicsPipeline {

		public:
			GraphicsPipeline();
			virtual ~GraphicsPipeline();
	
			virtual vk::Pipeline getHandle() = 0;
			virtual vk::RenderPass getRenderPass() = 0;
			virtual vk::Rect2D getRenderArea() = 0;
			virtual vk::PipelineLayout getLayout() = 0;
			virtual vk::DescriptorSetLayout getDescriptorSetLayout() = 0;

			virtual void recreate() = 0;
			virtual void onWindowResized(vk::Extent2D newWindowSize) = 0;

			virtual void cleanup() = 0;
		};
	}
}
