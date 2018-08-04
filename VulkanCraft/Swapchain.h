#pragma once
#include "GraphicalDevice.h"

namespace VulkanCraft {
	namespace Graphics {
		class Swapchain {
		public:
			Swapchain(const GraphicalDevice& gd, const vk::SurfaceKHR& surface);
			~Swapchain();

			void cleanup(const GraphicalDevice& gd);

			vk::SurfaceFormatKHR surfaceFormat;
			vk::PresentModeKHR presentMode;
			vk::Extent2D extent;

			vk::SwapchainKHR handle;
			std::vector<ImageView> imageViews;

		private: 
			vk::SurfaceFormatKHR getBestFormat(std::vector<vk::SurfaceFormatKHR> availableFormats);
			vk::PresentModeKHR getBestPresentMode(std::vector<vk::PresentModeKHR> availablePresentModes);
			vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR surfaceCapabilities);
		};
	}
}
