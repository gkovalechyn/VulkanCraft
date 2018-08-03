#include "GraphicalDevice.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

GraphicalDevice* GraphicalDevice::getGraphicalDevice(vk::Instance& instance, std::vector<const char*>& validationLayers) {
	GraphicalDevice* device = new GraphicalDevice();

	std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

	for (vk::PhysicalDevice physicalDevice : physicalDevices) {
		if (isDeviceSuitable(physicalDevice)) {
			device->physicalDevice = physicalDevice;
			break;
		}
	}

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device->physicalDevice.getQueueFamilyProperties();
	uint32_t queueFamilyIndex = 0;

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

		if (device->graphicsFamilyIndex > -1
			&& device->transferFamilyIndex > -1
			&& device->computeFamilyIndex > -1) {
			break;
		}
		queueFamilyIndex++;
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
		.setPQueueCreateInfos(queueCreateInfos.data());

	device->logicalDevice = device->physicalDevice.createDevice(deviceCreateInfo);

	device->graphicsQueue = device->logicalDevice.getQueue(device->graphicsFamilyIndex, 0);
	device->transferQueue = device->logicalDevice.getQueue(device->transferFamilyIndex, 0);
	device->computeQueue = device->logicalDevice.getQueue(device->computeFamilyIndex, 0);
	return device;
}

void VulkanCraft::Graphics::GraphicalDevice::destroy() {
	this->logicalDevice.destroy();
}

GraphicalDevice::GraphicalDevice() {
}


GraphicalDevice::~GraphicalDevice() {
}

bool VulkanCraft::Graphics::GraphicalDevice::isDeviceSuitable(vk::PhysicalDevice device) {
	vk::PhysicalDeviceProperties properties = device.getProperties();

	if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
		return true;
	}

	return false;
}
