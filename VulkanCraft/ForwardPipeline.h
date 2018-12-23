#pragma once
#include "GraphicsPipeline.h"
#include "json.hpp"
#include "ConfigurationError.h"

namespace VulkanCraft {
	namespace Graphics {

		class ForwardPipeline : public GraphicsPipeline {
		public:
			ForwardPipeline();
			~ForwardPipeline();

			ForwardPipeline(const ForwardPipeline& other) = delete;
			ForwardPipeline(ForwardPipeline&& other) = delete;

			// Inherited via GraphicsPipeline
			virtual vk::Pipeline getHandle() override;
			virtual void recreate() override;
			virtual void cleanup() override;

			static std::unique_ptr<ForwardPipeline> fromConfiguration(nlohmann::json file);

		private:
			std::string description;

			vk::ShaderModule vertexShaderModule;
			vk::ShaderModule tesselationShaderModule;
			vk::ShaderModule geometryShaderModule;
			vk::ShaderModule fragmentShaderModule;

			vk::Pipeline handle;

		};
	};
};

