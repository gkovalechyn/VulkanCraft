#include "GraphicalDevice.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

GraphicalDevice* GraphicalDevice::getGraphicalDevice(
	vk::Instance& instance,
	std::vector<const char*>& validationLayers, 
	std::vector<const char*>& requiredExtensions,
	vk::SurfaceKHR surface) {
	GraphicalDevice* device = new GraphicalDevice();

	std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

	for (vk::PhysicalDevice physicalDevice : physicalDevices) {
		if (isDeviceSuitable(physicalDevice, requiredExtensions, surface)) {
			device->physicalDevice = physicalDevice;
			break;
		}
	}

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device->physicalDevice.getQueueFamilyProperties();
	uint32_t queueFamilyIndex = 0;

#ifdef DEBUG
	Core::Logger::vaLog(Core::LogLevel::eDanger, "Queue families found: %d", queueFamilyProperties.size());
#endif

	for (auto queueFamilyProperty : queueFamilyProperties) {
		if (queueFamilyProperty.queueCount > 0 && (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics)) {
			device->graphicsFamilyIndex = queueFamilyIndex;
		}

		if (queueFamilyProperty.queueCount > 0 && (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer)) {
			device->transferFamilyIndex = queueFamilyIndex;
		}

		if (queueFamilyProperty.queueCount > 0 && (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute)) {
			device->computeFamilyIndex = queueFamilyIndex;
		}

		if (device->physicalDevice.getSurfaceSupportKHR(queueFamilyIndex, surface)) {
			device->presentFamilyIndex = queueFamilyIndex;
		}

		if (device->graphicsFamilyIndex > -1
			&& device->transferFamilyIndex > -1
			&& device->computeFamilyIndex > -1
			&& device->presentFamilyIndex > -1) {
			break;
		}
		queueFamilyIndex++;
	}

	if (device->graphicsFamilyIndex < 0
		|| device->transferFamilyIndex < 0
		|| device->computeFamilyIndex < 0
		|| device->presentFamilyIndex < 0) {
		Core::Logger::error("Could not find all family indexes");
	}
	//Swapchain stuff
	device->swapchainDetails.surfaceFormat = GraphicalDevice::getBestFormat(device->physicalDevice.getSurfaceFormatsKHR(surface));
	device->swapchainDetails.presentMode = GraphicalDevice::getBestPresentMode(device->physicalDevice.getSurfacePresentModesKHR(surface));
	device->swapchainDetails.extent = GraphicalDevice::chooseSwapExtent(device->physicalDevice.getSurfaceCapabilitiesKHR(surface));

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(3);
	float queuePriorities = 1.0f;

	queueCreateInfos[0]
		.setQueueCount(1)
		.setQueueFamilyIndex(device->graphicsFamilyIndex)
		.setPQueuePriorities(&queuePriorities);

	queueCreateInfos[1]
		.setQueueCount(1)
		.setQueueFamilyIndex(device->transferFamilyIndex)
		.setPQueuePriorities(&queuePriorities);

	queueCreateInfos[2]
		.setQueueCount(1)
		.setQueueFamilyIndex(device->computeFamilyIndex)
		.setPQueuePriorities(&queuePriorities);

	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.setEnabledLayerCount((uint32_t) validationLayers.size())
		.setPpEnabledLayerNames(validationLayers.data())
		.setQueueCreateInfoCount((uint32_t) queueCreateInfos.size())
		.setPQueueCreateInfos(queueCreateInfos.data())
		.setEnabledExtensionCount((uint32_t) requiredExtensions.size())
		.setPpEnabledExtensionNames(requiredExtensions.data());

	device->logicalDevice = device->physicalDevice.createDevice(deviceCreateInfo);

	device->graphicsQueue = device->logicalDevice.getQueue(device->graphicsFamilyIndex, 0);
	device->transferQueue = device->logicalDevice.getQueue(device->transferFamilyIndex, 0);
	device->computeQueue = device->logicalDevice.getQueue(device->computeFamilyIndex, 0);
	device->presentQueue = device->logicalDevice.getQueue(device->presentFamilyIndex, 0);

	return device;
}

void VulkanCraft::Graphics::GraphicalDevice::destroy() {
	this->logicalDevice.destroy();
}

GraphicalDevice::GraphicalDevice() {
}


GraphicalDevice::~GraphicalDevice() {
}

bool VulkanCraft::Graphics::GraphicalDevice::isDeviceSuitable(vk::PhysicalDevice device, std::vector<const char*>& requiredExtensions, vk::SurfaceKHR surface) {
	vk::PhysicalDeviceProperties properties = device.getProperties();

	if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
		std::vector<vk::ExtensionProperties> extensions = device.enumerateDeviceExtensionProperties();

		for (const char* requiredExtension : requiredExtensions) {
			bool found = false;

			for (vk::ExtensionProperties availableExtension : extensions) {
				if (strcmp(availableExtension.extensionName, requiredExtension) == 0) {
					found = true;
					break;
				}
			}

			if (!found) {
				return false;
			}
		}
	}

	std::vector<vk::SurfaceFormatKHR> surfaceFormats = device.getSurfaceFormatsKHR(surface);
	std::vector<vk::PresentModeKHR> presentModes = device.getSurfacePresentModesKHR(surface);

	if (surfaceFormats.empty() || presentModes.empty()) {
		return false;
	}

	return true;
}

vk::SurfaceFormatKHR VulkanCraft::Graphics::GraphicalDevice::getBestFormat(std::vector<vk::SurfaceFormatKHR> availableFormats) {
	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
		return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR VulkanCraft::Graphics::GraphicalDevice::getBestPresentMode(std::vector<vk::PresentModeKHR> availablePresentModes) {
	vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
			return availablePresentMode;
		} else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

vk::Extent2D VulkanCraft::Graphics::GraphicalDevice::chooseSwapExtent(vk::SurfaceCapabilitiesKHR capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		VkExtent2D actualExtent = capabilities.currentExtent;

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
