#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		class Shader {
		public:
			Shader();
			~Shader();

			void setStage(const vk::ShaderStageFlags stage);
			void setCode(const std::vector<char>& data);
			void build();

			vk::ShaderModule getHandle();
			vk::DescriptorSetLayout getDescriptorSetLayout();

			static Shader* loadFromFile(std::string file);

			friend class ShaderFactory;
		private:
			vk::DescriptorSetLayout layout;
			vk::ShaderModule handle;
			vk::ShaderStageFlags stage;
		};
	}
}