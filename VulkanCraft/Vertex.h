#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		struct Vertex {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
			glm::vec3 color;
		};
	}
}
