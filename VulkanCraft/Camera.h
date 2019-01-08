#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanCraft {
	namespace Graphics {
		class Camera {
		public:
			Camera();
			virtual ~Camera();

			glm::mat4 getViewProjectionMatrix();
		};
	}
}

