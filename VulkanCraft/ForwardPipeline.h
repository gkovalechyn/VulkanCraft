#pragma once
#include "GraphicsPipeline.h"
#include "json.hpp"
#include "ConfigurationError.h"
#include "FileUtils.h"

namespace VulkanCraft {
	namespace Graphics {

		class ForwardPipeline : public GraphicsPipeline {
		public:
			ForwardPipeline(const vk::Device& logicalDevice);
			~ForwardPipeline();

			ForwardPipeline(const ForwardPipeline& other) = delete;
			ForwardPipeline(ForwardPipeline&& other) = delete;

			// Inherited via GraphicsPipeline
			virtual vk::Pipeline getHandle() override;
			virtual void recreate() override;
			virtual void cleanup() override;

		private:
			const vk::Device& device;
			std::string description;

			struct ShaderModules {
				vk::ShaderModule vertex;
				vk::ShaderModule tesselation;
				vk::ShaderModule geometry;
				vk::ShaderModule fragment;
			} shaderModules;

			vk::Pipeline handle;

			void createShaderModules();
			void createPipeline();
		};
	};
};

