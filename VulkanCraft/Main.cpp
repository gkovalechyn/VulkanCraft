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


	auto renderingEngine = std::make_unique<VulkanCraft::Graphics::RenderingEngine>();
	renderingEngine->initialize(window);
	VulkanCraft::Graphics::Camera camera;
	auto resourceManager = renderingEngine->getResourceManager();

	auto shapes = VulkanCraft::Graphics::Mesh::fromOBJ("Resources/Models/Cube/Export/Cube.obj");
	resourceManager->uploadMeshToGPU(*shapes[0]);

	VulkanCraft::TestRenderable testRenderable;
	testRenderable.setMesh(std::move(shapes[0]));
	testRenderable.setPipeline(renderingEngine->getDefaultPipeline());
	//testRenderable.setResourceManager(resourceManager);

	auto defaultPipeline = renderingEngine->getDefaultPipeline();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		resourceManager->updateTransfers();

		renderingEngine->beginFrame();
		renderingEngine->beginPass(*defaultPipeline, camera);

		renderingEngine->queueForRendering(testRenderable);

		renderingEngine->endPass();
		renderingEngine->endFrame(resourceManager->getImportantPendingTransfers());
	}

	renderingEngine.reset();
	glfwDestroyWindow(window);
	glfwTerminate();
}