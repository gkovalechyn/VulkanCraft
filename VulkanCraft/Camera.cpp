#include "Camera.h"

VulkanCraft::Graphics::Camera::Camera() {
}

VulkanCraft::Graphics::Camera::~Camera() {
}

glm::mat4 VulkanCraft::Graphics::Camera::getViewProjectionMatrix() {
	glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800 / (float)600, 0.1f, 10.0f);

	return view * projection;
}
