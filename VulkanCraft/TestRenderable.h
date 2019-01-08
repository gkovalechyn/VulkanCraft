#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "Renderable.h"
#include "Mesh.h"
#include "libs/tiny_obj_loader.h"
#include "Logger.h"

namespace VulkanCraft {
	class TestRenderable : public Graphics::Renderable {
	public:
		TestRenderable();
		~TestRenderable();
	private:
		glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), 1 * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	};
}

