#pragma once

#include "GraphicsPipeline.h"
#include "FileUtils.h"
#include "GraphicalDevice.h"

namespace VulkanCraft {
	namespace Graphics {
		class DefaultPipeline : public GraphicsPipeline{
		public:
			DefaultPipeline(GraphicalDevice& device);
			~DefaultPipeline();

			vk::Pipeline getHandle();
			void recreate();
			void cleanup();

			void setVertexShader();
			void setFragmentShader();

			void build();

		private:
			GraphicalDevice * device;
			vk::Pipeline handle;

			vk::ShaderModule vertexShaderModule;
			vk::ShaderModule fragmentShaderModule;

			void createShaderModules();
		};
	}
}

