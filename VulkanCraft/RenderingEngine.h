#pragma once
#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "GlobalDefines.h"
#include "Logger.h"
#include "GraphicalDevice.h"
#include "Swapchain.h"
#include "DefaultPipeline.h"

namespace VulkanCraft {
	namespace Graphics {
		class RenderingEngine {
		public:
			RenderingEngine();
			~RenderingEngine();

			void initialize(GLFWwindow* window);
			void initializeVulkan();
			void initializeGraphicalDevice();

			void terminate();
		private:
			GLFWwindow * window;
			vk::Instance vkInstance;
			VkDebugReportCallbackEXT debugCallbackHandle;
			GraphicalDevice* device;
			vk::SurfaceKHR surface;
			Swapchain* swapchain;
			GraphicsPipeline* pipeline;

#ifdef DEBUG
			std::vector<const char*> requiredLayers = { "VK_LAYER_LUNARG_standard_validation" };
#else
			std::vector<const char*> requiredLayers = {};
#endif// DEBUG

			
		};
	}
}