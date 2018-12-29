#include <iostream>
#include "Logger.h"
#include "RenderingEngine.h"

int main(int argc, char* argv[]) {
	VulkanCraft::Core::Logger::createInstance();
	int logargc = 0;

	glfwInit();

	GLFWwindow* window;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);

	
	VulkanCraft::Graphics::RenderingEngine* renderingEngine = new VulkanCraft::Graphics::RenderingEngine();
	renderingEngine->initialize(window);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	delete renderingEngine;
	glfwDestroyWindow(window);
	glfwTerminate();
}