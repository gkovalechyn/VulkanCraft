#pragma once
#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "GlobalDefines.h"
#include "Logger.h"
#include "GraphicalDevice.h"
#include "Swapchain.h"
#include "GraphicsPipeline.h"
#include "ForwardPipeline.h"

namespace VulkanCraft {
	namespace Graphics {
		struct PerFrameData {
			vk::Framebuffer framebuffer;
			vk::CommandBuffer commandBuffer;
		};

		struct WindowData {
			GLFWwindow * handle;
			vk::SurfaceKHR surface;
			vk::SurfaceFormatKHR surfaceFormat;
			vk::PresentModeKHR presentMode;
			vk::Extent2D surfaceExtent;
		};

		class RenderingEngine {
		public:
			RenderingEngine();
			~RenderingEngine();

			RenderingEngine(const RenderingEngine& other) = delete;
			RenderingEngine(RenderingEngine&& other) = delete;

			void initialize(GLFWwindow* window);
			

			void registerPipeline(std::string name, GraphicsPipeline* pipeline);
		private:
			vk::Instance vkInstance;
			VkDebugReportCallbackEXT debugCallbackHandle;
			std::unique_ptr<GraphicalDevice> device;
			std::unique_ptr<Swapchain> swapchain;
			std::unique_ptr<GraphicsPipeline> pipeline;

			WindowData window;
			std::vector<PerFrameData> frames;
			
			

#ifdef DEBUG
			std::vector<const char*> requiredLayers = { "VK_LAYER_LUNARG_standard_validation" };
#else
			std::vector<const char*> requiredLayers = {};
#endif// DEBUG

			void initializeVulkan();
			void initializeGraphicalDevice();
			void chooseBestWindowParameters();
		};
	}
}