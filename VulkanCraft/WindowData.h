#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		struct WindowData {
			GLFWwindow * handle;
			vk::SurfaceKHR surface;
			vk::SurfaceFormatKHR surfaceFormat;
			vk::PresentModeKHR presentMode;
			vk::Extent2D surfaceExtent;
		};
	}
}