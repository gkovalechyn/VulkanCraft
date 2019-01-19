#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		struct DescriptorSetData {
			vk::DescriptorSet descriptorSet;
			vk::DescriptorType type;
			uint32_t set;
			uint32_t binding;

			bool dirty;

			union Info {
				vk::DescriptorBufferInfo bufferInfo;
				vk::DescriptorImageInfo imageInfo;
				vk::BufferView texelView;
			} info;

			union Data {
				vk::Buffer buffer;
				vk::ImageView image;
			} data;
		};

		class GraphicsPipeline {

		public:
			GraphicsPipeline();
			virtual ~GraphicsPipeline();
	
			virtual vk::Pipeline getHandle() = 0;
			virtual vk::RenderPass getRenderPass() = 0;
			virtual vk::Rect2D getRenderArea() = 0;
			virtual vk::PipelineLayout getLayout() = 0;
			virtual std::vector<DescriptorSetData> allocateDescriptorSets() = 0;
			virtual DescriptorSetData allocateDescriptorSet(uint32_t set, uint32_t index) = 0;

			virtual void recreate() = 0;
			virtual void cleanup() = 0;

		};
	}
}
