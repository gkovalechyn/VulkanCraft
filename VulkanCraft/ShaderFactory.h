#pragma once
#include <fstream>
#include <vulkan/vulkan.hpp>
#include "Shader.h"
#include "json.hpp"
#include "FileUtils.h"

using json = nlohmann::json;

namespace VulkanCraft {
	namespace Graphics {
		class ShaderFactory {
		public:
			ShaderFactory(const vk::Device& device) : device(device) {};
			~ShaderFactory();

			ShaderFactory(const ShaderFactory& other) = delete;
			ShaderFactory(ShaderFactory&& other) = delete;

			Shader* loadFromConfiguration(nlohmann::json shaderConfig);
			Shader* getShader(const std::string& name) noexcept;
		private:
			vk::Device device;

			std::unordered_map<std::string, Shader*> loadedShaders;
		};
	}
}

