#include <iostream>
#include <GLFW/glfw3.h>
#include "Logger.h"
#include "RenderingEngine.h"

int main(int argc, char* argv[]) {
	VulkanCraft::Core::Logger::createInstance();

	glfwInit();

	GLFWwindow* window;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);

	
	VulkanCraft::Graphics::RenderingEngine renderingEngine;
	renderingEngine.initialize(window);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	renderingEngine.terminate();
	glfwDestroyWindow(window);
	glfwTerminate();
}