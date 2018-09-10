#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		class Shader {
		public:
			Shader();
			~Shader();

			void setStage(vk::ShaderStageFlags stage);
			void setCode();
			void build();

			vk::ShaderModule getHandle();
			vk::DescriptorSetLayout getDescriptorSetLayout();

			static Shader* loadFromFile(std::string file);
		private:
			vk::DescriptorSetLayout layout;
			vk::ShaderModule handle;
			vk::ShaderStageFlags stage;
		};
	}
}