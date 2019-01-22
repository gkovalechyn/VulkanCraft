#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "Renderable.h"
#include "Mesh.h"
#include "libs/tiny_obj_loader.h"
#include "Logger.h"

namespace VulkanCraft {
	class TestRenderable : public Graphics::Renderable {
	public:
		TestRenderable();
		~TestRenderable();

		// Inherited via Renderable
		virtual const glm::mat4 & getModelMatrix() override;
		void update();

		void setOffset(glm::vec3 offset) {
			this->positionOffset = offset;
		}

		void setRotationSpeed(float speed) {
			this->rotationSpeed = speed;
		}

	private:
		glm::vec3 positionOffset;
		float rotationSpeed;

		std::chrono::time_point<std::chrono::steady_clock> startTime;
		glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), 1 * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	};
}

