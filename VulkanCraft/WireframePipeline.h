#pragma once
#include "GraphicsPipeline.h"
namespace VulkanCraft {
	namespace Graphics {
		class WireframePipeline : public GraphicsPipeline {
		public:
			WireframePipeline();
			~WireframePipeline();

			// Inherited via GraphicsPipeline
			virtual vk::Pipeline getHandle() override;
			virtual vk::RenderPass getRenderPass() override;
			virtual void recreate() override;
			virtual void cleanup() override;
		};
	}
}

