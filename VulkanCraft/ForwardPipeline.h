#pragma once
#include "GraphicsPipeline.h"
#include "json.hpp"
#include "ConfigurationError.h"
#include "FileUtils.h"
#include "Vertex.h"

namespace VulkanCraft {
	namespace Graphics {

		class ForwardPipeline : public GraphicsPipeline {
		public:
			ForwardPipeline(const vk::Device& logicalDevice, const vk::Extent2D viewport);
			~ForwardPipeline();

			ForwardPipeline(const ForwardPipeline& other) = delete;
			ForwardPipeline(ForwardPipeline&& rhs);

			void setViewport(vk::Extent2D viewport) noexcept;
			vk::Extent2D getViewport() noexcept;

			// Inherited via GraphicsPipeline
			virtual vk::Pipeline getHandle() override;
			virtual void recreate() override;
			virtual void cleanup() override;

		private:
			const vk::Device& device;
			vk::Extent2D viewport;

			std::string description;

			struct ShaderModules {
				vk::ShaderModule vertex;
				vk::ShaderModule tesselation;
				vk::ShaderModule geometry;
				vk::ShaderModule fragment;
			} shaderModules;

			vk::PipelineLayout layout;
			vk::RenderPass renderPass;
			vk::Pipeline handle;

			void createShaderModules();
			void createLayout();
			void createRenderPass();
			void createPipeline();
		};
	};
};

