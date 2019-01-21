#include "ForwardPipeline.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

ForwardPipeline::ForwardPipeline(const vk::Device& logicalDevice, const WindowData& windowData) : device(logicalDevice), windowData{ windowData }{
	this->createShaderModules();
	this->createLayout();
	this->createRenderPass();
	this->createPipeline();
}


ForwardPipeline::~ForwardPipeline() {
	Core::Logger::debug("Destroying forward pipeline");

	this->device.destroyPipeline(this->handle);
	this->device.destroyPipelineLayout(this->layout);
	this->device.destroyDescriptorSetLayout(this->descriptorSetLayout);

	
	this->device.destroyShaderModule(this->shaderModules.vertex);
	this->device.destroyShaderModule(this->shaderModules.fragment);	

	Core::Logger::debug("Finished destroying forward pipeline");
}

VulkanCraft::Graphics::ForwardPipeline::ForwardPipeline(ForwardPipeline && rhs): device(rhs.device), windowData (rhs.windowData) {
	this->description = std::move(rhs.description);
	this->shaderModules.vertex = rhs.shaderModules.vertex;
	this->shaderModules.fragment = rhs.shaderModules.fragment;
	
	this->handle = rhs.handle;
	this->layout = rhs.layout;
	this->renderPass = rhs.renderPass;

	rhs.layout = nullptr;
	rhs.handle = nullptr;
	rhs.renderPass = nullptr;

	rhs.shaderModules.vertex = nullptr;
	rhs.shaderModules.fragment = nullptr;
}

void VulkanCraft::Graphics::ForwardPipeline::setViewport(vk::Extent2D viewport) noexcept {
	this->windowData.surfaceExtent = viewport;
}

vk::Extent2D VulkanCraft::Graphics::ForwardPipeline::getViewport() noexcept {
	return this->windowData.surfaceExtent;
}

vk::Pipeline ForwardPipeline::getHandle() {
	return vk::Pipeline();
}

void ForwardPipeline::recreate() {
}

void ForwardPipeline::cleanup() {
}

void VulkanCraft::Graphics::ForwardPipeline::createShaderModules() {
	vk::ShaderModuleCreateInfo vertexCreateInfo;
	vk::ShaderModuleCreateInfo fragmentCreateInfo;

	std::vector<char> vertexCode = Core::FileUtils::readFileEntirely("Resources/Shaders/forward.vert.spv");
	std::vector<char> fragmentCode = Core::FileUtils::readFileEntirely("Resources/Shaders/forward.frag.spv");

	vertexCreateInfo
		.setCodeSize(vertexCode.size() * sizeof(char))
		.setPCode(reinterpret_cast<const uint32_t*>(vertexCode.data()));

	fragmentCreateInfo
		.setCodeSize(fragmentCode.size() * sizeof(char))
		.setPCode(reinterpret_cast<const uint32_t*>(fragmentCode.data()));

	this->shaderModules.vertex = this->device.createShaderModule(vertexCreateInfo);
	this->shaderModules.fragment = this->device.createShaderModule(fragmentCreateInfo);
}

void VulkanCraft::Graphics::ForwardPipeline::createLayout() {
	vk::PushConstantRange pushConstants;
	pushConstants
		.setStageFlags(vk::ShaderStageFlagBits::eVertex)
		.setSize(sizeof(glm::mat4))
		.setOffset(0);

	/*
	vk::DescriptorSetLayoutBinding descriptorBinding0;
	descriptorBinding0
		.setBinding(0)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);
	*/
	vk::DescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo;
	descriptorLayoutCreateInfo
		.setBindingCount(0);
		//.setPBindings(&descriptorBinding0);

		
	this->descriptorSetLayout = this->device.createDescriptorSetLayout(descriptorLayoutCreateInfo);

	vk::PipelineLayoutCreateInfo layoutCreateInfo;
	layoutCreateInfo
		.setSetLayoutCount(1)
		.setPSetLayouts(&this->descriptorSetLayout)
		.setPushConstantRangeCount(1)
		.setPPushConstantRanges(&pushConstants);
		
	this->layout = this->device.createPipelineLayout(layoutCreateInfo);
}

void VulkanCraft::Graphics::ForwardPipeline::createRenderPass() {
	

	vk::AttachmentDescription colorAttachment;
	colorAttachment
		.setFormat(this->windowData.surfaceFormat.format)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::SubpassDependency colorAttachmentDependency;
	colorAttachmentDependency
		.setSrcSubpass(VK_SUBPASS_EXTERNAL)
		.setDstSubpass(0)
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

	vk::AttachmentReference colorReference;
	colorReference
		.setAttachment(0)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription subpass;
	subpass
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&colorReference);

	vk::RenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo
		.setAttachmentCount(1)
		.setPAttachments(&colorAttachment)
		.setDependencyCount(1)
		.setPDependencies(&colorAttachmentDependency)
		.setSubpassCount(1)
		.setPSubpasses(&subpass);

	this->renderPass = this->device.createRenderPass(renderPassCreateInfo);
}

void VulkanCraft::Graphics::ForwardPipeline::createPipeline() {
	vk::PipelineShaderStageCreateInfo vertexStageCreateInfo;
	vk::PipelineShaderStageCreateInfo fragmentStageCreateInfo;

	vertexStageCreateInfo
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setPName("main")
		.setModule(this->shaderModules.vertex);

	fragmentStageCreateInfo
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setPName("main")
		.setModule(this->shaderModules.fragment);

	//Vertex input
	vk::VertexInputBindingDescription vertexInputBindingDescription;
	vertexInputBindingDescription
		.setBinding(0)
		.setInputRate(vk::VertexInputRate::eVertex)
		.setStride(sizeof(Vertex));

	vk::VertexInputAttributeDescription posAttributeDescription;
	vk::VertexInputAttributeDescription normalAttributeDescription;
	vk::VertexInputAttributeDescription uvAttributeDescription;
	vk::VertexInputAttributeDescription colorAttributeDescription;

	posAttributeDescription
		.setBinding(0)
		.setFormat(vk::Format::eR32G32B32Sfloat)
		.setLocation(0)
		.setOffset(offsetof(Vertex, position));

	normalAttributeDescription
		.setBinding(0)
		.setFormat(vk::Format::eR32G32B32Sfloat)
		.setLocation(1)
		.setOffset(offsetof(Vertex, normal));

	uvAttributeDescription
		.setBinding(0)
		.setFormat(vk::Format::eR32G32Sfloat)
		.setLocation(2)
		.setOffset(offsetof(Vertex, uv));

	colorAttributeDescription
		.setBinding(0)
		.setFormat(vk::Format::eR32G32B32Sfloat)
		.setLocation(3)
		.setOffset(offsetof(Vertex, color));

	vk::VertexInputAttributeDescription attributes[] = { posAttributeDescription, normalAttributeDescription, uvAttributeDescription, colorAttributeDescription };
	vk::VertexInputBindingDescription bindings[] = { vertexInputBindingDescription };

	vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
	vertexInputStateCreateInfo
		.setVertexAttributeDescriptionCount(4)
		.setPVertexAttributeDescriptions(attributes)

		.setVertexBindingDescriptionCount(1)
		.setPVertexBindingDescriptions(bindings);

	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	inputAssemblyInfo
		.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(true);

	//Viewport
	vk::Viewport viewport;
	viewport
		.setX(0)
		.setY(0)
		.setMinDepth(0)
		.setMaxDepth(1)
		.setWidth(static_cast<float>(this->windowData.surfaceExtent.width))
		.setHeight(static_cast<float>(this->windowData.surfaceExtent.height));

	vk::Rect2D scissorRect;
	scissorRect
		.setOffset({ 0, 0 })
		.setExtent(this->windowData.surfaceExtent);

	vk::PipelineViewportStateCreateInfo viewportCreateInfo;
	viewportCreateInfo
		.setScissorCount(1)
		.setPScissors(&scissorRect)
		.setViewportCount(1)
		.setPViewports(&viewport);

	//Rasterization
	vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo;
	rasterizationCreateInfo
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setFrontFace(vk::FrontFace::eClockwise)
		.setDepthClampEnable(false)
		.setPolygonMode(vk::PolygonMode::eFill)

		.setLineWidth(1.0f)

		.setDepthBiasEnable(false);

	//Multisampling
	vk::PipelineMultisampleStateCreateInfo multisamplingCreateInfo;
	multisamplingCreateInfo
		.setSampleShadingEnable(false)
		.setRasterizationSamples(vk::SampleCountFlagBits::e1);

	//Color blending
	vk::PipelineColorBlendAttachmentState blendAttachment;
	blendAttachment
		.setBlendEnable(false)
		.setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB);

	vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo;
	colorBlendCreateInfo
		.setLogicOp(vk::LogicOp::eNoOp)
		.setLogicOpEnable(false)
		.setAttachmentCount(1)
		.setPAttachments(&blendAttachment);
	
	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertexStageCreateInfo, fragmentStageCreateInfo };
	vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo
		.setLayout(this->layout)
		.setStageCount(2)
		.setPStages(shaderStages)
		.setRenderPass(this->renderPass)
		.setSubpass(0)
		.setPVertexInputState(&vertexInputStateCreateInfo)
		.setPInputAssemblyState(&inputAssemblyInfo)
		.setPColorBlendState(&colorBlendCreateInfo)
		.setPViewportState(&viewportCreateInfo)
		.setPRasterizationState(&rasterizationCreateInfo)
		.setPMultisampleState(&multisamplingCreateInfo);

	this->handle = this->device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
}

void VulkanCraft::Graphics::ForwardPipeline::createPerFrameData() {
}

vk::DescriptorSetLayout VulkanCraft::Graphics::ForwardPipeline::getDescriptorSetLayout() {
	return this->descriptorSetLayout;
}

vk::PipelineLayout VulkanCraft::Graphics::ForwardPipeline::getLayout() {
	return this->layout;
}
vk::RenderPass VulkanCraft::Graphics::ForwardPipeline::getRenderPass() {
	return this->renderPass;
}

vk::Rect2D VulkanCraft::Graphics::ForwardPipeline::getRenderArea() {
	return vk::Rect2D({ 0, 0 }, this->windowData.surfaceExtent);
}