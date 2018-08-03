#pragma once
#include <vulkan/vulkan.hpp>
namespace VulkanCraft {
	namespace Graphics {
		struct GraphicalDevice {
		public:
			vk::PhysicalDevice physicalDevice;
			vk::Device logicalDevice;
			uint32_t graphicsFamilyIndex = -1;
			uint32_t transferFamilyIndex = -1;
			uint32_t computeFamilyIndex = -1;

			vk::Queue graphicsQueue;
			vk::Queue transferQueue;
			vk::Queue computeQueue;

			static GraphicalDevice* getGraphicalDevice(vk::Instance& instance, std::vector<const char*>& validationLayers);
			void destroy();

		private:
			GraphicalDevice();
			~GraphicalDevice();

			static bool isDeviceSuitable(vk::PhysicalDevice device);
		};
	}
}

