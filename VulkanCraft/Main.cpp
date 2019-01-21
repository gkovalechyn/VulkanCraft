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
	std::vector<VulkanCraft::TestRenderable> objects;

	for (auto& shape : shapes) {
		resourceManager->uploadMeshToGPU(*shape);
		VulkanCraft::TestRenderable testRenderable;
		testRenderable.setMesh(std::move(shape));

		objects.push_back(testRenderable);
	}

	auto defaultPipeline = renderingEngine->getDefaultPipeline();


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		for (auto& renderable : objects) {
			renderable.update();
		}

		resourceManager->updateTransfers();

		renderingEngine->beginFrame();
		renderingEngine->beginPass(*defaultPipeline, camera);

		for (auto& renderable : objects) {
			renderingEngine->queueForRendering(renderable);
		}

		renderingEngine->endPass();
		renderingEngine->endFrame(resourceManager->getImportantPendingTransfers());

		std::stringstream newWindowName;
		newWindowName << "VulkanCraft " << renderingEngine->getFPS() << "fps (" << renderingEngine->getFrameTime() << "ms)";
		glfwSetWindowTitle(window, newWindowName.str().c_str());
	}
	
	renderingEngine.reset();
	glfwDestroyWindow(window);
	glfwTerminate();
}