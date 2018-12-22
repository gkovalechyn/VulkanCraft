#pragma once
#include "GraphicsPipeline.h"
namespace VulkanCraft {
	namespace Graphics {

		class ForwardPipeline : public GraphicsPipeline {
		public:
			ForwardPipeline();
			~ForwardPipeline();

			// Inherited via GraphicsPipeline
			virtual vk::Pipeline getHandle() override;
			virtual void recreate() override;
			virtual void cleanup() override;
		};
	};
};

