#include "RenderingEngine.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;
	
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	Core::Logger::debug(std::string(layerPrefix) + ":" + msg);

	return VK_FALSE;
}

RenderingEngine::RenderingEngine() {
}


RenderingEngine::~RenderingEngine() {
}

void RenderingEngine::initialize(GLFWwindow * window) {
	this->window = window;
	this->initializeVulkan();
	this->initializeGraphicalDevice();
}

void RenderingEngine::initializeVulkan() {
	vk::ApplicationInfo applicationInfo;
	applicationInfo.setApiVersion(VK_API_VERSION_1_0)
		.setPApplicationName("VulkanCraft")
		.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
		.setPEngineName("No engine")
		.setEngineVersion(VK_MAKE_VERSION(0, 0, 1));

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	
#ifdef DEBUG
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif // DEBUG


	vk::InstanceCreateInfo createInfo;
	createInfo.setPApplicationInfo(&applicationInfo)
		.setEnabledExtensionCount((uint32_t) extensions.size())
		.setPpEnabledExtensionNames(extensions.data());

#ifdef DEBUG
	createInfo.setEnabledLayerCount((uint32_t) requiredLayers.size())
		.setPpEnabledLayerNames(requiredLayers.data());
#endif

	this->vkInstance = vk::createInstance(createInfo, nullptr);

#ifdef DEBUG
	VkDebugReportCallbackCreateInfoEXT  debugCreateInfo;
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	debugCreateInfo.pfnCallback = &debugCallback;

	PFN_vkCreateDebugReportCallbackEXT createDebugReportCallbackFunction = (PFN_vkCreateDebugReportCallbackEXT) this->vkInstance.getProcAddr("vkCreateDebugReportCallbackEXT");
	createDebugReportCallbackFunction(
		(VkInstance) this->vkInstance,
		&debugCreateInfo,
		nullptr,
		&debugCallbackHandle
	);

	Core::Logger::debug("Created debug report callback");
#endif
}

void VulkanCraft::Graphics::RenderingEngine::initializeGraphicalDevice() {
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(this->vkInstance, this->window, nullptr, &surface);
	this->surface = surface;

	std::vector<const char*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	this->device = GraphicalDevice::getGraphicalDevice(this->vkInstance, requiredLayers, requiredExtensions, this->surface);

	this->swapchain = std::make_unique<Swapchain>(*this->device, this->surface);
}

void RenderingEngine::terminate() {
	this->swapchain->cleanup(*this->device);
	this->vkInstance.destroySurfaceKHR(this->surface);

#ifdef DEBUG
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)this->vkInstance.getProcAddr("vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func((VkInstance)this->vkInstance, this->debugCallbackHandle, nullptr);
	}
#endif // DEBUG

	this->vkInstance.destroy();
}
