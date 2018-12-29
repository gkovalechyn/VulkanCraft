#pragma once
#include "GraphicalDevice.h"

namespace VulkanCraft {
	namespace Graphics {
		class Swapchain {
		public:
			Swapchain(const GraphicalDevice& gd, const vk::SurfaceKHR& surface, const vk::SurfaceFormatKHR format, const vk::PresentModeKHR presentMode, const vk::Extent2D surfaceExtent);
			~Swapchain();

			vk::SwapchainKHR handle;
			std::vector<ImageView> imageViews;

		private: 
			const GraphicalDevice& device;
		};
	}
}
