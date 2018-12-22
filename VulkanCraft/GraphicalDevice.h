#pragma once
#include <vulkan/vulkan.hpp>
#include "Logger.h"
#include "GraphicsDefs.h"

namespace VulkanCraft {
	namespace Graphics {
		struct GraphicalDevice {
		public:
			GraphicalDevice();
			~GraphicalDevice();

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

			/**
				Finds a graphical device that has the given validation layers, extensions required and that can also present to the given surface.
			*/
			static std::unique_ptr<GraphicalDevice> getGraphicalDevice(vk::Instance& instance,
				std::vector<const char*>& validationLayers,
				std::vector<const char*>& requiredExtensions,
				vk::SurfaceKHR surface);
			void destroy();
		private:
			

			static bool isDeviceSuitable(vk::PhysicalDevice device, std::vector<const char*>& requiredExtensions, vk::SurfaceKHR surface);
			
		};
	}
}

