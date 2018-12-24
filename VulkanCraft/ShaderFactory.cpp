#include "ShaderFactory.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

ShaderFactory::~ShaderFactory() {

}

Shader * VulkanCraft::Graphics::ShaderFactory::createFromConfiguration(vk::ShaderStageFlags stage, nlohmann::json shaderConfig) {
	std::string path = shaderConfig["path"].get<std::string>();
	auto fileData = Core::FileUtils::readFileEntirely(path);
	std::unique_ptr<Shader> shader = std::make_unique<Shader>();

	vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
	shaderModuleCreateInfo.codeSize = fileData.size() * sizeof(char);
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fileData.data());

	vk::ShaderModule shaderModule = this->device.createShaderModule(shaderModuleCreateInfo);

	shader->stage = stage;
	shader->code = fileData;
	shader->handle = shaderModule;

	throw std::exception("NOT IMPLEMENTED");

	return nullptr;
}

Shader * VulkanCraft::Graphics::ShaderFactory::getShader(const std::string & name) noexcept {
	auto iterator = this->loadedShaders.find(name);

	if (iterator != this->loadedShaders.end()) {
		return iterator->second;
	}

	return nullptr;
}
