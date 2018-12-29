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

			/**
			<summary>
				Creates a shader instance and caches it based on the configuration json object
			</summary>

			<param name="stage">Which stage the shader belongs to</param>
			<param name="shaderConfig">JSON object that is the shader configuration</param>
			*/
			Shader* createFromConfiguration(vk::ShaderStageFlags stage, nlohmann::json shaderConfig);

			/**
			<summary>
				Gets a currently cached shader instance.
			</summary>

			<returns>A reference to a shader if it is cached, nullptr otherwise.</returns>
			*/
			Shader* getShader(const std::string& name) noexcept;
		private:
			vk::Device device;

			std::unordered_map<std::string, Shader*> loadedShaders;
		};
	}
}

