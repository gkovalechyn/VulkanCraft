#include "Swapchain.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;


Swapchain::Swapchain(const GraphicalDevice& gd, const vk::SurfaceKHR& surface) {
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = gd.physicalDevice.getSurfaceCapabilitiesKHR(surface);

	this->surfaceFormat = this->getBestFormat(gd.physicalDevice.getSurfaceFormatsKHR(surface));
	this->presentMode = this->getBestPresentMode(gd.physicalDevice.getSurfacePresentModesKHR(surface));
	this->extent = this->chooseSwapExtent(surfaceCapabilities);

	uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
		imageCount = surfaceCapabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.setMinImageCount(imageCount)
		.setSurface(surface)
		.setImageFormat(this->surfaceFormat.format)
		.setImageColorSpace(this->surfaceFormat.colorSpace)
		.setImageExtent(this->extent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setPreTransform(surfaceCapabilities.currentTransform)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(this->presentMode)
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

	for (int i = 0; i < swapchainImages.size(); i++) {
		vk::ImageViewCreateInfo ivCreateInfo;

		ivCreateInfo.setImage(swapchainImages[i])
			.setFormat(this->surfaceFormat.format)
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
}

void VulkanCraft::Graphics::Swapchain::cleanup(const GraphicalDevice& gd) {
	for (ImageView view : this->imageViews) {
		gd.logicalDevice.destroyImageView(view.view);
	}

	gd.logicalDevice.destroySwapchainKHR(this->handle);
}

vk::SurfaceFormatKHR Swapchain::getBestFormat(std::vector<vk::SurfaceFormatKHR> availableFormats) {
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

vk::PresentModeKHR Swapchain::getBestPresentMode(std::vector<vk::PresentModeKHR> availablePresentModes) {
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

vk::Extent2D Swapchain::chooseSwapExtent(vk::SurfaceCapabilitiesKHR capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		VkExtent2D actualExtent = capabilities.currentExtent;

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}