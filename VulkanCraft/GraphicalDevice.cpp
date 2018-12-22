#include "GraphicalDevice.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

std::unique_ptr<GraphicalDevice> GraphicalDevice::getGraphicalDevice(
	vk::Instance& instance,
	std::vector<const char*>& validationLayers, 
	std::vector<const char*>& requiredExtensions,
	vk::SurfaceKHR surface) {
	auto device = std::make_unique<GraphicalDevice>();

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


