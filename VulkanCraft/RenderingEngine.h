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

namespace VulkanCraft {
	namespace Graphics {

		class RenderingEngine {
		public:
			RenderingEngine();
			~RenderingEngine();

			RenderingEngine(const RenderingEngine& other) = delete;
			RenderingEngine(RenderingEngine&& other) = delete;

			void initialize(GLFWwindow* window);
			

			void registerPipeline(std::string name, GraphicsPipeline* pipeline);

			void terminate();
		private:
			GLFWwindow * window;
			vk::Instance vkInstance;
			VkDebugReportCallbackEXT debugCallbackHandle;
			std::unique_ptr<GraphicalDevice> device;
			vk::SurfaceKHR surface;
			std::unique_ptr<Swapchain> swapchain;
			//std::unique_ptr<GraphicsPipeline> pipeline;

#ifdef DEBUG
			std::vector<const char*> requiredLayers = { "VK_LAYER_LUNARG_standard_validation" };
#else
			std::vector<const char*> requiredLayers = {};
#endif// DEBUG

			void initializeVulkan();
			void initializeGraphicalDevice();
		};
	}
}