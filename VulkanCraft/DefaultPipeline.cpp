#include "DefaultPipeline.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

DefaultPipeline::DefaultPipeline(GraphicalDevice& device) {
	this->device = &device;

	this->createShaderModules();

	vk::PipelineShaderStageCreateInfo vertexCreateInfo;
	vk::PipelineShaderStageCreateInfo fragmentCreateInfo;

	vertexCreateInfo
		.setModule(this->vertexShaderModule)
		.setPName("main")
		.setStage(vk::ShaderStageFlagBits::eVertex);

	fragmentCreateInfo
		.setModule(this->fragmentShaderModule)
		.setPName("main")
		.setStage(vk::ShaderStageFlagBits::eFragment);
}


DefaultPipeline::~DefaultPipeline() {

}

vk::Pipeline VulkanCraft::Graphics::DefaultPipeline::getHandle() {
	return this->handle;
}

void VulkanCraft::Graphics::DefaultPipeline::cleanup() {
	this->device->logicalDevice.destroyShaderModule(this->vertexShaderModule);
	this->device->logicalDevice.destroyShaderModule(this->fragmentShaderModule);
}

void VulkanCraft::Graphics::DefaultPipeline::createShaderModules() {
	std::vector<char> vertexShaderCode = Core::FileUtils::readFileEntirely("");
	std::vector<char> fragmentShaderCode = Core::FileUtils::readFileEntirely("");

	vk::ShaderModuleCreateInfo vertexCreateInfo;
	vertexCreateInfo.setCodeSize(vertexShaderCode.size())
		.setPCode(reinterpret_cast<uint32_t*>(vertexShaderCode.data()));


	vk::ShaderModuleCreateInfo fragmentCreateInfo;
	vertexCreateInfo.setCodeSize(fragmentShaderCode.size())
		.setPCode(reinterpret_cast<uint32_t*>(fragmentShaderCode.data()));

	this->vertexShaderModule = this->device->logicalDevice.createShaderModule(vertexCreateInfo);
	this->fragmentShaderModule = this->device->logicalDevice.createShaderModule(fragmentCreateInfo);
}

