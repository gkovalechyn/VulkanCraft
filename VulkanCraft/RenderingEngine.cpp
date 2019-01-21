#include "RenderingEngine.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {
	Core::Logger::debug(std::string(layerPrefix) + ": " + msg);
	//Core::Logger::vaLog(Core::LogLevel::eDebug, "%s: %s", std::string(layerPrefix), msg);

	return VK_FALSE;
}

RenderingEngine::RenderingEngine() {
	
}


RenderingEngine::~RenderingEngine() {
	Core::Logger::debug("Destroying rendering engine");
	this->destroyPerFrameData();

	this->device->logicalDevice.destroyCommandPool(this->commandPool);

	this->pipeline.reset();
	delete this->swapchain;

	this->vkInstance.destroySurfaceKHR(this->window.surface);

	delete this->device;

#ifdef DEBUG
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)this->vkInstance.getProcAddr("vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func((VkInstance)this->vkInstance, this->debugCallbackHandle, nullptr);
	}
#endif // DEBUG

	this->vkInstance.destroy();

	Core::Logger::debug("Finished destroying rendering engine");
}

void RenderingEngine::initialize(GLFWwindow * window) {
	this->window.handle = window;
	this->initializeVulkan();
	this->initializeGraphicalDevice();

	this->pipeline = std::make_shared<ForwardPipeline>(this->device->logicalDevice, this->window.surfaceExtent);
	Core::Logger::debug("Created forward pipeline");

	vk::CommandPoolCreateInfo commandPoolCreateInfo;
	commandPoolCreateInfo
		.setQueueFamilyIndex(this->device->graphicsFamilyIndex)
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	this->commandPool = this->device->logicalDevice.createCommandPool(commandPoolCreateInfo);

	this->createPerFrameData();

	this->resourceManager = std::make_unique<VulkanCraft::Graphics::ResourceManager>(*this->device, this->getDefaultPipeline()->getDescriptorSetLayout());

	this->resourceManager->createModelUbo(256);
}

void VulkanCraft::Graphics::RenderingEngine::beginFrame() {
	this->currentFrame = (this->currentFrame + 1) % this->frames.size();
	const PerFrameData& frame = this->frames[this->currentFrame];

	auto fenceStatus = this->device->logicalDevice.getFenceStatus(frame.inFlightFence);
	if (fenceStatus == vk::Result::eNotReady) {

		this->device->logicalDevice.waitForFences({
				frame.inFlightFence
			},
			true,
			std::numeric_limits<uint64_t>().max());
	}


	this->device->logicalDevice.resetFences({ frame.inFlightFence });
	frame.commandBuffer.reset({});

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	frame.commandBuffer.begin(beginInfo);

	this->resourceManager->mapModelDynamicUbo(&this->modelUboMemory);
}

void VulkanCraft::Graphics::RenderingEngine::beginPass(GraphicsPipeline & pipeline, Camera& camera) {
	const PerFrameData& frame = this->frames[this->currentFrame];

	vk::RenderPassBeginInfo renderPassInfo;
	vk::ClearValue clearValue;
	clearValue.setColor(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});

	//TODO: Add support for multiple render passes and multiple pipelines
	renderPassInfo
		.setClearValueCount(1)
		.setPClearValues(&clearValue)
		.setFramebuffer(frame.framebuffer)
		.setRenderArea(this->pipeline->getRenderArea())
		.setRenderPass(this->pipeline->getRenderPass());

	frame.commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	frame.commandBuffer.pushConstants<glm::mat4>(pipeline.getLayout(), vk::ShaderStageFlagBits::eVertex, 0, { camera.getViewProjectionMatrix() });

	frame.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.getHandle());
}

void VulkanCraft::Graphics::RenderingEngine::queueForRendering(Renderable & renderable) {
	const PerFrameData& frame = this->frames[this->currentFrame];

	/*
	if (renderable.areDescriptorSetsDirty()) {
		std::vector<vk::WriteDescriptorSet> writes(renderable.getDescriptorSets().size());

		for (const auto& descriptorData : renderable.getDescriptorSets()) {
			vk::WriteDescriptorSet writeDescriptorSet = {};

			writeDescriptorSet
				.setDescriptorCount(1)
				.setDescriptorType(descriptorData.type)
				.setDstBinding(descriptorData.binding)
				.setDstSet(descriptorData.descriptorSet);

			switch (descriptorData.type) {
				case vk::DescriptorType::eUniformBuffer:
				case vk::DescriptorType::eStorageBuffer:
				case vk::DescriptorType::eStorageBufferDynamic:
					writeDescriptorSet.setPBufferInfo(&descriptorData.info.bufferInfo);
					break;

				case vk::DescriptorType::eSampler:
				case vk::DescriptorType::eStorageImage:
					writeDescriptorSet.setPImageInfo(&descriptorData.info.imageInfo);
					break;

				case vk::DescriptorType::eStorageTexelBuffer:
					writeDescriptorSet.setPTexelBufferView(&descriptorData.info.texelView);
					break;

				default:
					throw std::runtime_error("Unkown descriptor type");
			}

			writes.push_back(writeDescriptorSet);
		}


		this->device->logicalDevice.updateDescriptorSets(writes, {});
	}
	*/

	/*
	std::vector<vk::DescriptorSet> descriptorSets(renderable.getDescriptorSets().size());

	for (const auto& descriptorSetWithData : renderable.getDescriptorSets()) {
		descriptorSets.push_back(descriptorSetWithData.descriptorSet);
	}
	*/
	auto mesh = renderable.getMesh();
	auto renderData = renderable.getRenderData();
	
	auto modelMatrix = renderable.getModelMatrix();

	renderData.uboIndex = 0;
	memcpy(this->modelUboMemory, &modelMatrix, sizeof(glm::mat4));

	frame.commandBuffer.bindVertexBuffers(0, { mesh->getVertexBuffer().buffer }, { mesh->getVertexBuffer().offset });
	frame.commandBuffer.bindIndexBuffer(mesh->getIndexBuffer().buffer, mesh->getIndexBuffer().offset, vk::IndexType::eUint32);
	frame.commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, this->pipeline->getLayout(), 0, {this->resourceManager->getModelDescriptorSet()}, {renderData.uboIndex});

	frame.commandBuffer.drawIndexed(static_cast<uint32_t>(mesh->getVertices().size()), 1, 0, 0, 0);
}

void VulkanCraft::Graphics::RenderingEngine::endPass() {
	PerFrameData& frame = this->frames[this->currentFrame];

	frame.commandBuffer.endRenderPass();
}

void VulkanCraft::Graphics::RenderingEngine::endFrame(const std::vector<PendingMemoryTransfer>& pendingTransfers) {
	const PerFrameData& frame = this->frames[this->currentFrame];

	frame.commandBuffer.end();

	//One for each transfer that needs to finish and 1 more for the image available semaphore
	std::vector<vk::PipelineStageFlags> waitStages(pendingTransfers.size() + 1);
	std::vector<vk::Semaphore> waitSemaphores(pendingTransfers.size() + 1);

	uint32_t imageIndex = this->device->logicalDevice.acquireNextImageKHR(this->swapchain->handle, std::numeric_limits<uint64_t>::max(), frame.imageAvailableSemaphore, nullptr).value;

	for (const auto& pendingTransfer : pendingTransfers) {
		waitSemaphores.push_back(pendingTransfer.doneSemaphore);
		waitStages.push_back(vk::PipelineStageFlagBits::eVertexShader);
	}

	waitSemaphores.push_back(frame.imageAvailableSemaphore);
	waitStages.push_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	vk::SubmitInfo submitInfo;
	submitInfo
		.setCommandBufferCount(1)
		.setPCommandBuffers(&frame.commandBuffer)
		.setWaitSemaphoreCount(static_cast<uint32_t>(waitSemaphores.size()))
		.setPWaitSemaphores(waitSemaphores.data())
		.setPWaitDstStageMask(waitStages.data())
		.setSignalSemaphoreCount(1)
		.setPSignalSemaphores(&frame.renderingDoneSemaphore);

	this->device->graphicsQueue.submit(submitInfo, frame.inFlightFence);

	vk::PresentInfoKHR presentInfo;

	presentInfo
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&frame.imageAvailableSemaphore)
		.setSwapchainCount(1)
		.setPSwapchains(&this->swapchain->handle)
		.setPImageIndices(&imageIndex);

	this->device->graphicsQueue.presentKHR(presentInfo);

	this->resourceManager->unmapModelDynamicUbo();

	std::stringstream newTitle;
	newTitle << "VulkanCraft frame " << this->currentFrame;
	glfwSetWindowTitle(this->window.handle, newTitle.str().c_str());
}

GraphicalDevice * VulkanCraft::Graphics::RenderingEngine::getDevice() {
	return this->device;
}

ResourceManager * VulkanCraft::Graphics::RenderingEngine::getResourceManager() {
	return this->resourceManager.get();
}

std::shared_ptr<GraphicsPipeline> VulkanCraft::Graphics::RenderingEngine::getDefaultPipeline() noexcept {
	return this->pipeline;
}

void RenderingEngine::initializeVulkan() {
	vk::ApplicationInfo applicationInfo;
	applicationInfo.setApiVersion(VK_API_VERSION_1_0)
		.setPApplicationName("VulkanCraft")
		.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
		.setPEngineName("No engine")
		.setEngineVersion(VK_MAKE_VERSION(0, 0, 1));

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef DEBUG
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	extensions.push_back("VK_EXT_debug_utils");
#endif // DEBUG
	for (const auto& layerProperty : vk::enumerateInstanceLayerProperties()) {
		Core::Logger::vaLog(Core::LogLevel::eDebug, "Available layer: %s, spec %d", layerProperty.layerName, layerProperty.specVersion);
	}

	for (const auto& extensionProperty : vk::enumerateInstanceExtensionProperties()) {
		Core::Logger::vaLog(Core::LogLevel::eDebug, "Available extension: %s, spec %d", extensionProperty.extensionName, extensionProperty.specVersion);
	}

	vk::InstanceCreateInfo createInfo;
	createInfo.setPApplicationInfo(&applicationInfo)
		.setEnabledExtensionCount((uint32_t)extensions.size())
		.setPpEnabledExtensionNames(extensions.data());

#ifdef DEBUG
	createInfo.setEnabledLayerCount((uint32_t)requiredLayers.size())
		.setPpEnabledLayerNames(requiredLayers.data());
#endif

	this->vkInstance = vk::createInstance(createInfo, nullptr);

#ifdef DEBUG
	VkDebugReportCallbackCreateInfoEXT  debugCreateInfo;
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	debugCreateInfo.pfnCallback = &debugCallback;

	PFN_vkCreateDebugReportCallbackEXT createDebugReportCallbackFunction = (PFN_vkCreateDebugReportCallbackEXT)this->vkInstance.getProcAddr("vkCreateDebugReportCallbackEXT");
	createDebugReportCallbackFunction(
		(VkInstance)this->vkInstance,
		&debugCreateInfo,
		nullptr,
		&debugCallbackHandle
	);

	Core::Logger::debug("Created debug report callback");
#endif
}

void VulkanCraft::Graphics::RenderingEngine::initializeGraphicalDevice() {
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(this->vkInstance, this->window.handle, nullptr, &surface);
	this->window.surface = surface;

	std::vector<const char*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	this->device = GraphicalDevice::getGraphicalDevice(this->vkInstance, requiredLayers, requiredExtensions, this->window.surface);

	this->chooseBestWindowParameters();

	this->swapchain = new Swapchain(*this->device, this->window.surface, this->window.surfaceFormat, this->window.presentMode, this->window.surfaceExtent);
}

void VulkanCraft::Graphics::RenderingEngine::chooseBestWindowParameters() {
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = this->device->physicalDevice.getSurfaceCapabilitiesKHR(this->window.surface);
	std::vector<vk::SurfaceFormatKHR> availableFormats = this->device->physicalDevice.getSurfaceFormatsKHR(this->window.surface);

	//Surface format
	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
		Core::Logger::debug("Any format supported, picking best format: vk::Format::eB8G8R8A8Unorm");
		this->window.surfaceFormat = { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			this->window.surfaceFormat = availableFormat;
			break;
		}
	}
	if (this->window.surfaceFormat.format == vk::Format::eUndefined) {
		Core::Logger::error("Could not find suitable surface format, selecting first available format");
		this->window.surfaceFormat = availableFormats[0];
	}

	//Present mode
	vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;
	std::vector<vk::PresentModeKHR> availablePresentModes = this->device->physicalDevice.getSurfacePresentModesKHR(this->window.surface);

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
			bestMode = availablePresentMode;
			break;
		} else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
			bestMode = availablePresentMode;
		}
	}
	this->window.presentMode = bestMode;

	//Surface extent
	vk::SurfaceCapabilitiesKHR capabilities = this->device->physicalDevice.getSurfaceCapabilitiesKHR(this->window.surface);
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		this->window.surfaceExtent = capabilities.currentExtent;
	} else {
		VkExtent2D actualExtent = capabilities.currentExtent;

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		this->window.surfaceExtent = actualExtent;
	}
}

void VulkanCraft::Graphics::RenderingEngine::createPerFrameData() {
	this->frames.resize(this->swapchain->imageViews.size());

	vk::CommandBufferAllocateInfo allocationInfo;
	allocationInfo
		.setCommandBufferCount(static_cast<uint32_t>(this->frames.size()))
		.setCommandPool(this->commandPool)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	std::vector<vk::CommandBuffer> commandBuffers = this->device->logicalDevice.allocateCommandBuffers(allocationInfo);

	vk::DescriptorPoolSize poolSize;
	poolSize
		.setDescriptorCount(2048)
		.setType(vk::DescriptorType::eStorageBuffer);

	vk::DescriptorPoolCreateInfo createInfo;
	createInfo
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
		.setPoolSizeCount(1)
		.setPPoolSizes(&poolSize);



	for (int i = 0; i < this->frames.size(); i++) {
		PerFrameData& data = this->frames[i];
		vk::ImageView attachments[] = { this->swapchain->imageViews[i].view };

		vk::FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo
			.setAttachmentCount(1)
			.setPAttachments(attachments)
			.setHeight(this->window.surfaceExtent.height)
			.setWidth(this->window.surfaceExtent.width)
			.setRenderPass(this->pipeline->getRenderPass())
			.setLayers(1);

		vk::SemaphoreCreateInfo semaphoreCreateInfo;
		vk::FenceCreateInfo fenceCreateInfo;
		fenceCreateInfo
			.setFlags(vk::FenceCreateFlagBits::eSignaled);

		data.framebuffer = this->device->logicalDevice.createFramebuffer(framebufferCreateInfo);
		data.commandBuffer = commandBuffers[i];
		data.imageAvailableSemaphore = this->device->logicalDevice.createSemaphore(semaphoreCreateInfo);
		data.renderingDoneSemaphore = this->device->logicalDevice.createSemaphore(semaphoreCreateInfo);
		data.inFlightFence = this->device->logicalDevice.createFence(fenceCreateInfo);

		data.descriptorPool = this->device->logicalDevice.createDescriptorPool(createInfo);
	}
}

void VulkanCraft::Graphics::RenderingEngine::destroyPerFrameData() {
	for (const PerFrameData& data : this->frames) {
		this->device->logicalDevice.destroySemaphore(data.imageAvailableSemaphore);
		this->device->logicalDevice.destroySemaphore(data.renderingDoneSemaphore);
		this->device->logicalDevice.destroyFence(data.inFlightFence);

		this->device->logicalDevice.destroyFramebuffer(data.framebuffer);
		this->device->logicalDevice.freeCommandBuffers(this->commandPool, data.commandBuffer);
	}
}
