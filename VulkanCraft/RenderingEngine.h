#pragma once
#include <string>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "GlobalDefines.h"
#include "Logger.h"
#include "GraphicalDevice.h"

namespace VulkanCraft {
	namespace Graphics {
		class RenderingEngine {
		public:
			RenderingEngine();
			~RenderingEngine();

			void initialize(GLFWwindow* window);
			void initializeVulkan();

			void terminate();
		private:
			GLFWwindow * window;
			vk::Instance vkInstance;
			VkDebugReportCallbackEXT debugCallbackHandle;
			GraphicalDevice* device;

		};
	}
}