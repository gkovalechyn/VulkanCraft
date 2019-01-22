#include <iostream>
#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "TestRenderable.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

void glfwErrorCallback(int error, const char* message) {
	VulkanCraft::Core::Logger::error(std::to_string(error));
	VulkanCraft::Core::Logger::error(message);
}

int main(int argc, char* argv[]) {
	VulkanCraft::Core::Logger::createInstance();
	float speed = 0.3;

	glfwSetErrorCallback(&glfwErrorCallback);

	VulkanCraft::Core::Logger::debug("Initializing GLFW");
	glfwInit();

	VulkanCraft::Core::Logger::debug("Creating window");
	GLFWwindow* window;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);

	auto renderingEngine = std::make_unique<VulkanCraft::Graphics::RenderingEngine>();
	renderingEngine->initialize(window);
	VulkanCraft::Graphics::Camera camera;
	auto resourceManager = renderingEngine->getResourceManager();

	auto shapes = VulkanCraft::Graphics::Mesh::fromOBJ("Resources/Models/Teapot/Teapot.obj");
	std::shared_ptr<VulkanCraft::Graphics::Mesh> cubeMesh = std::move(shapes[0]);
	std::vector<VulkanCraft::TestRenderable> objects;

	resourceManager->uploadMeshToGPU(*cubeMesh);

	for (auto i = 0; i < 1; i++) {
		VulkanCraft::Core::Logger::vaLog(VulkanCraft::Core::LogLevel::eDebug, "Offset: %d", (i * 3) - 15);

		VulkanCraft::TestRenderable testRenderable;
		auto modelAllocation = resourceManager->allocateModelData(sizeof(glm::mat4));

		testRenderable.setMesh(cubeMesh);
		testRenderable.getRenderData().modelAllocation = modelAllocation;
		testRenderable.setRotationSpeed(speed);
		testRenderable.setOffset(glm::vec3(i * 3, i * 0, i * 0));

		objects.push_back(testRenderable);
	}

	auto defaultPipeline = renderingEngine->getDefaultPipeline();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
			VulkanCraft::Core::Logger::debug("Key add pressed");
			speed += 0.01;

			for (auto i = 0; i < objects.size(); i++) {
				objects[i].setRotationSpeed(i * speed);
			}
		}

		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
			VulkanCraft::Core::Logger::debug("Key subtract pressed");
			speed -= 0.01;

			for (auto i = 0; i < objects.size(); i++) {
				objects[i].setRotationSpeed(i * speed);
			}
		}

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