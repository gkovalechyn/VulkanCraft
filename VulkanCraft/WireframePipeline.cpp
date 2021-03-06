#include "WireframePipeline.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

WireframePipeline::WireframePipeline() {
}


WireframePipeline::~WireframePipeline() {
}

vk::Pipeline VulkanCraft::Graphics::WireframePipeline::getHandle() {
	return vk::Pipeline();
}

vk::RenderPass VulkanCraft::Graphics::WireframePipeline::getRenderPass() {
	return vk::RenderPass();
}

void VulkanCraft::Graphics::WireframePipeline::recreate() {
}

void VulkanCraft::Graphics::WireframePipeline::cleanup() {
}
