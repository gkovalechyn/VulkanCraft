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

	const std::vector<VulkanCraft::Graphics::Vertex> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0

	};
	//auto shapes = VulkanCraft::Graphics::Mesh::fromOBJ("Resources/Models/Cube/Export/Cube.obj");
	auto mesh = std::make_shared< VulkanCraft::Graphics::Mesh>(vertices, indices);
	resourceManager->uploadMeshToGPU(*mesh);

	VulkanCraft::TestRenderable testRenderable;
	testRenderable.setMesh(std::move(mesh));
	testRenderable.setPipeline(renderingEngine->getDefaultPipeline());
	//testRenderable.setResourceManager(resourceManager);

	auto defaultPipeline = renderingEngine->getDefaultPipeline();



	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		testRenderable.update();

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