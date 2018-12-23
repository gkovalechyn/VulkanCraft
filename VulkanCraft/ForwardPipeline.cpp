#include "ForwardPipeline.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

ForwardPipeline::ForwardPipeline() {
}


ForwardPipeline::~ForwardPipeline() {
}

vk::Pipeline ForwardPipeline::getHandle() {
	return vk::Pipeline();
}

void ForwardPipeline::recreate() {
}

void ForwardPipeline::cleanup() {
}

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
	auto vertexShaderCode = nullptr;

	if (shadersObject.find("fragment") == shadersObject.end()) {
		throw Core::ConfigurationError("Shaders object is missing the fragment stage");
	}

	return pipelinePtr;
}
