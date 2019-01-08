#include <iostream>
#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "TestRenderable.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

int main(int argc, char* argv[]) {
	VulkanCraft::Core::Logger::createInstance();
	int logargc = 0;

	glfwInit();

	GLFWwindow* window;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);


	VulkanCraft::Graphics::RenderingEngine* renderingEngine = new VulkanCraft::Graphics::RenderingEngine();
	renderingEngine->initialize(window);
	VulkanCraft::Graphics::Camera camera;
	VulkanCraft::Graphics::ResourceManager* resourceManager = new VulkanCraft::Graphics::ResourceManager(
		renderingEngine->getDevice()->physicalDevice,
		renderingEngine->getDevice()->logicalDevice,
		renderingEngine->getDevice()->graphicsQueue
	);

	VulkanCraft::TestRenderable testRenderable;

	VulkanCraft::Graphics::GraphicsPipeline* pipeline = renderingEngine->getDefaultPipeline();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		renderingEngine->beginFrame();
		renderingEngine->beginPass(*pipeline, camera);

		renderingEngine->queueForRendering(testRenderable);

		renderingEngine->endPass();
		renderingEngine->endFrame();
	}

	delete renderingEngine;
	glfwDestroyWindow(window);
	glfwTerminate();
}