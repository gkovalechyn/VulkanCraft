#include "ShaderFactory.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

ShaderFactory::~ShaderFactory() {

}

Shader * VulkanCraft::Graphics::ShaderFactory::loadFromConfiguration(vk::ShaderStageFlags stage, nlohmann::json shaderConfig) {
	std::string path = shaderConfig["path"].get<std::string>();
	auto fileData = Core::FileUtils::readFileEntirely(path);
	std::unique_ptr<Shader> shader = std::make_unique<Shader>();

	shader->setStage(stage);
	shader->setCode(fileData);

	
}

Shader * VulkanCraft::Graphics::ShaderFactory::getShader(const std::string & name) noexcept{
	auto iterator = this->loadedShaders.find(name);

	if (iterator != this->loadedShaders.end()) {
		return iterator->second;
	}

	return nullptr;
}
