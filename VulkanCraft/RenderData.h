#pragma once
#include "Mesh.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanCraft {
	namespace Graphics {
		struct ModelUBOData {
			glm::vec3 position;
			glm::vec3 scale;
			glm::quat rotation;
		};

		struct RenderData {
			ModelUBOData modelUboData;
			GPUAllocation modelAllocation;

			glm::mat4 calculateModelMatrix() {
				auto scale = glm::scale(glm::mat4(1), this->modelUboData.scale);
				auto rotation = glm::mat4_cast(this->modelUboData.rotation);
				auto translation = glm::translate(glm::mat4(1), this->modelUboData.position);

				return scale * rotation * translation;
			}
		};
	}
}