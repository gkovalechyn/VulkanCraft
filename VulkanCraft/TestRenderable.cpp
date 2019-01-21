#include "TestRenderable.h"

VulkanCraft::TestRenderable::TestRenderable() {
	this->startTime = std::chrono::high_resolution_clock::now();
}


VulkanCraft::TestRenderable::~TestRenderable() {

}

const glm::mat4 & VulkanCraft::TestRenderable::getModelMatrix() {
	return this->modelMatrix;
}

void VulkanCraft::TestRenderable::update() {
	auto currentTime = std::chrono::high_resolution_clock::now();
	float timePassed = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - this->startTime).count();

	this->modelMatrix = glm::rotate(glm::mat4(1.0f), timePassed * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}
