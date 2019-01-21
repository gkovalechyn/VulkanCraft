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

	auto shapes = VulkanCraft::Graphics::Mesh::fromOBJ("Resources/Models/Teapot/Teapot.obj");
	resourceManager->uploadMeshToGPU(*shapes[0]);

	VulkanCraft::TestRenderable testRenderable;
	testRenderable.setMesh(std::move(shapes[0]));
	testRenderable.setPipeline(renderingEngine->getDefaultPipeline());
	//testRenderable.setResourceManager(resourceManager);

	auto defaultPipeline = renderingEngine->getDefaultPipeline();

	/*
	auto vma = resourceManager->getAllocator();
	
	vk::BufferCreateInfo vertexCreateInfo = {};
	vertexCreateInfo
		.setSize(64 * 1024)
		.setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer);
	VkBufferCreateInfo proxy = vertexCreateInfo;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;
	allocInfo.requiredFlags = (VkMemoryPropertyFlags) (vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);

	VkBuffer handle;
	VmaAllocation allocation;
	VmaAllocationInfo info;
	vmaCreateBuffer(vma, &proxy, &allocInfo, &handle, &allocation, &info);

	auto mesh = testRenderable.getMesh();
	void* data;
	vmaMapMemory(vma, allocation, &data);
	auto vertexBufferSize = sizeof(VulkanCraft::Graphics::Vertex) * mesh->getVertexCount();
	memcpy(data, mesh->getVertices().data(), vertexBufferSize);

	uint8_t* indexStart = ((uint8_t*)data) + vertexBufferSize;
	uint32_t meshIndices[] = { 0, 1, 2 };
	memcpy(indexStart, meshIndices, sizeof(uint32_t) * 3);
	

	auto vertexAllocation = VulkanCraft::Graphics::GPUAllocation(0, handle, vertexBufferSize, 0);
	auto indexAllocation = VulkanCraft::Graphics::GPUAllocation(1, handle, sizeof(uint32_t) * 3, vertexBufferSize);
	
	mesh->setVertexBuffer(vertexAllocation);
	mesh->setIndexBuffer(indexAllocation);
	*/

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