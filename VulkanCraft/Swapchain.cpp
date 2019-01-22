#include "Swapchain.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;


Swapchain::Swapchain(const GraphicalDevice& gd, const vk::SurfaceKHR& surface, const vk::SurfaceFormatKHR format, const vk::PresentModeKHR presentMode, const vk::Extent2D surfaceExtent): device(gd) {
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = gd.physicalDevice.getSurfaceCapabilitiesKHR(surface);

	uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
		imageCount = surfaceCapabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.setMinImageCount(imageCount)
		.setSurface(surface)
		.setImageFormat(format.format)
		.setImageColorSpace(format.colorSpace)
		.setImageExtent(surfaceExtent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setPreTransform(surfaceCapabilities.currentTransform)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(presentMode)
		.setClipped(true)
		.setOldSwapchain(nullptr);

	if (gd.graphicsFamilyIndex != gd.presentFamilyIndex) {
		uint32_t familyIndices[2] = {gd.graphicsFamilyIndex, gd.presentFamilyIndex};

		createInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
			.setQueueFamilyIndexCount(2)
			.setPQueueFamilyIndices(familyIndices);
	}else{
		createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	}

	this->handle = gd.logicalDevice.createSwapchainKHR(createInfo);

	std::vector<vk::Image> swapchainImages = gd.logicalDevice.getSwapchainImagesKHR(this->handle);
	this->imageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		vk::ImageViewCreateInfo ivCreateInfo;

		ivCreateInfo.setImage(swapchainImages[i])
			.setFormat(format.format)
			.setViewType(vk::ImageViewType::e2D);

		vk::ComponentMapping mapping;
		mapping.setA(vk::ComponentSwizzle::eIdentity);
		mapping.setR(vk::ComponentSwizzle::eIdentity);
		mapping.setG(vk::ComponentSwizzle::eIdentity);
		mapping.setB(vk::ComponentSwizzle::eIdentity);

		ivCreateInfo.setComponents(mapping);

		vk::ImageSubresourceRange range;
		range.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setBaseMipLevel(0)
			.setLayerCount(1)
			.setLevelCount(1);

		ivCreateInfo.setSubresourceRange(range);	
		
		vk::ImageView iv = gd.logicalDevice.createImageView(ivCreateInfo);

		this->imageViews[i].view = iv;
		this->imageViews[i].image = swapchainImages[i];
	}
}


Swapchain::~Swapchain() {
	for (ImageView view : this->imageViews) {
		this->device.logicalDevice.destroyImageView(view.view);
	}

	this->device.logicalDevice.destroySwapchainKHR(this->handle);
}