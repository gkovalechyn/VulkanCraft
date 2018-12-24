#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace VulkanCraft {
	namespace Graphics {
		struct Vertex {
			glm::vec3 position;
			glm::vec2 uv;
		};
	}
}
