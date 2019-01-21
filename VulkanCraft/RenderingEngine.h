#pragma once
#include <string>
#include "libs/vk_mem_alloc.h" //Here to prevent the APIENTRY macro redefinition
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "GlobalDefines.h"
#include "Logger.h"
#include "GraphicalDevice.h"
#include "Swapchain.h"
#include "GraphicsPipeline.h"
#include "ForwardPipeline.h"
#include "Renderable.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "RenderData.h"
#include <sstream>
#include <corecrt_malloc.h>

namespace VulkanCraft {
	namespace Graphics {
		struct PerFrameData {
			vk::Framebuffer framebuffer;
			vk::CommandBuffer commandBuffer;
			vk::Semaphore imageAvailableSemaphore;
			vk::Semaphore renderingDoneSemaphore;
			vk::Fence inFlightFence;
			vk::DescriptorPool descriptorPool;
		};

		struct WindowData {
			GLFWwindow * handle;
			vk::SurfaceKHR surface;
			vk::SurfaceFormatKHR surfaceFormat;
			vk::PresentModeKHR presentMode;
			vk::Extent2D surfaceExtent;
		};

		typedef VkBool32(VKAPI_PTR *PFN_vkDebugUtilsMessengerCallbackEXT)(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		class RenderingEngine {
		public:
			RenderingEngine();
			~RenderingEngine();

			RenderingEngine(const RenderingEngine& other) = delete;

			void initialize(GLFWwindow* window);
			
			void beginFrame();
			void beginPass(GraphicsPipeline& pipeline, Camera& camera);
			void queueForRendering(Renderable& renderable);
			void endPass();
			void endFrame(const std::vector<PendingMemoryTransfer>& pendingTransfers);

			GraphicalDevice* getDevice();
			ResourceManager* getResourceManager();

			void registerPipeline(std::string name, GraphicsPipeline* pipeline);
			std::shared_ptr<GraphicsPipeline> getDefaultPipeline() noexcept;
		private:
			vk::Instance vkInstance;
			VkDebugUtilsMessengerEXT debugMessenger;
			GraphicalDevice* device;
			WindowData window;

			Swapchain* swapchain;

			//This needs to be a shared pointer due to objects referencing the pipeline that should be used to draw them
			//and they need to know it for them to be able to allocated the proper descriptor sets when their pipeline changes
			std::shared_ptr<GraphicsPipeline> pipeline;
			std::unique_ptr<ResourceManager> resourceManager;
			vk::CommandPool commandPool;
			void* modelUboMemory;

			std::vector<PerFrameData> frames;
			int currentFrame = 0;

#ifdef DEBUG
			std::vector<const char*> requiredLayers = { "VK_LAYER_LUNARG_standard_validation"};
			std::vector<const char*> requiredExtensions = { "VK_EXT_debug_report" };
#else
			std::vector<const char*> requiredLayers = {};
			std::vector<const char*> requiredExtensions = {};
#endif// DEBUG

			void initializeVulkan();
			void initializeGraphicalDevice();
			void chooseBestWindowParameters();
			void createPerFrameData();

			void destroyPerFrameData();
		};
	}
}