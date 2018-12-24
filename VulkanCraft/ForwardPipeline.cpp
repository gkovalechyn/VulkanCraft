#include "ForwardPipeline.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

ForwardPipeline::ForwardPipeline(const vk::Device& logicalDevice) : device(logicalDevice){
	this->createShaderModules();
}


ForwardPipeline::~ForwardPipeline() {
	this->device.destroyShaderModule(this->shaderModules.vertex);
	this->device.destroyShaderModule(this->shaderModules.fragment);
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

	vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
}


/*
std::unique_ptr<ForwardPipeline> VulkanCraft::Graphics::ForwardPipeline::fromConfiguration(nlohmann::json file) {
	auto pipelinePtr = std::make_unique<ForwardPipeline>();

	pipelinePtr->description = file["description"].get<std::string>();

	if (file.find("shaders") == file.end()) {
		throw Core::ConfigurationError("Pipeline configuration file is missing shaders object");
	}

	auto shadersObject = file["shaders"];

	if (shadersObject.find("vertex") == shadersObject.end()) {
		throw Core::ConfigurationError("Shaders object is missing the vertex stage");
	}

	if (shadersObject.find("fragment") == shadersObject.end()) {
		throw Core::ConfigurationError("Shaders object is missing the fragment stage");
	}

	return pipelinePtr;
}
*/