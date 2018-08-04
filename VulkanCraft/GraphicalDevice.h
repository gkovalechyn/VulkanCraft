#pragma once
#include <vulkan/vulkan.hpp>
#include "Logger.h"

namespace VulkanCraft {
	namespace Graphics {
		struct GraphicalDevice {
		public:
			vk::PhysicalDevice physicalDevice;
			vk::Device logicalDevice;
			uint32_t graphicsFamilyIndex = -1;
			uint32_t transferFamilyIndex = -1;
			uint32_t computeFamilyIndex = -1;
			uint32_t presentFamilyIndex = -1;

			vk::Queue graphicsQueue;
			vk::Queue transferQueue;
			vk::Queue computeQueue;
			vk::Queue presentQueue;

			struct swapchainDetails {
				vk::SurfaceFormatKHR surfaceFormat;
				vk::PresentModeKHR presentMode;
				vk::Extent2D extent;
			} swapchainDetails;

			/**
				Finds a graphical device that has the given validation layers, extensions required and that can also present to the given surface.
			*/
			static GraphicalDevice* getGraphicalDevice(vk::Instance& instance,
				std::vector<const char*>& validationLayers,
				std::vector<const char*>& requiredExtensions,
				vk::SurfaceKHR surface);
			void destroy();

		private:
			GraphicalDevice();
			~GraphicalDevice();

			static bool isDeviceSuitable(vk::PhysicalDevice device, std::vector<const char*>& requiredExtensions, vk::SurfaceKHR surface);
			static vk::SurfaceFormatKHR getBestFormat(std::vector<vk::SurfaceFormatKHR> availableFormats);
			static vk::PresentModeKHR getBestPresentMode(std::vector<vk::PresentModeKHR> availablePresentModes);
			static vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR surfaceCapabilities);
		};
	}
}

