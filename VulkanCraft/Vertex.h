#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		typedef struct Vertex {
			glm::vec3 position;
			glm::vec2 uv;
		}Vertex;
	}
}
