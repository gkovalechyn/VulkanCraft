#include "Camera.h"

VulkanCraft::Graphics::Camera::Camera() {
}

VulkanCraft::Graphics::Camera::~Camera() {
}

glm::mat4 VulkanCraft::Graphics::Camera::getViewProjectionMatrix() {
	glm::mat4 view = glm::lookAt(glm::vec3(0, 5, -10), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1280 / (float)720, 0.1f, 10.0f);

	projection[1][1] *= -1;

	return projection * view;
}
