#pragma once
#include "GraphicsPipeline.h"
#include "libs/json.hpp"
#include "ConfigurationError.h"
#include "FileUtils.h"
#include "Vertex.h"
#include "Logger.h"
#include "WindowData.h"

namespace VulkanCraft {
	namespace Graphics {

		class ForwardPipeline : public GraphicsPipeline {
		public:
			ForwardPipeline(const vk::Device& logicalDevice, const WindowData& windowData);
			~ForwardPipeline();

			ForwardPipeline(const ForwardPipeline& other) = delete;
			ForwardPipeline(ForwardPipeline&& rhs);

			void setViewport(vk::Extent2D viewport) noexcept;
			vk::Extent2D getViewport() noexcept;

			// Inherited via GraphicsPipeline
			virtual vk::Pipeline getHandle() override;
			virtual vk::RenderPass getRenderPass() override;
			virtual vk::Rect2D getRenderArea() override;
			virtual vk::PipelineLayout getLayout() override;
			virtual vk::DescriptorSetLayout getDescriptorSetLayout() override;
			virtual void onWindowResized(vk::Extent2D newWindowSize) override;

			virtual void recreate() override;
			virtual void cleanup() override;

		private:
			const vk::Device& device;
			WindowData windowData;

			std::string description;

			struct ShaderModules {
				vk::ShaderModule vertex;
				vk::ShaderModule tesselation;
				vk::ShaderModule geometry;
				vk::ShaderModule fragment;
			} shaderModules;

			vk::DescriptorSetLayout descriptorSetLayout;
			vk::PipelineLayout layout;

			vk::RenderPass renderPass;

			vk::Pipeline handle;

			void createShaderModules();
			void createLayout();
			void createRenderPass();
			void createPipeline();
			void createPerFrameData();

			

			

		};
	};
};

