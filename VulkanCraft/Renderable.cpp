#include "Renderable.h"

using namespace VulkanCraft::Graphics;

uint32_t VulkanCraft::Graphics::Renderable::getInstanceNumber() {
	return this->instanceId;
}


std::vector<DescriptorSetData>& VulkanCraft::Graphics::Renderable::getDescriptorSets() {
	return this->validDescriptorSets;
}

bool VulkanCraft::Graphics::Renderable::areDescriptorSetsDirty() {
	return this->descriptorSetsAreDirty;
}

std::shared_ptr<Mesh> VulkanCraft::Graphics::Renderable::getMesh() {
	return this->mesh;
}

void VulkanCraft::Graphics::Renderable::setMesh(const std::shared_ptr<Mesh>& mesh) {
	this->mesh = mesh;
}

bool VulkanCraft::Graphics::Renderable::hasMesh() {
	return this->mesh != nullptr;
}

std::shared_ptr<GraphicsPipeline> VulkanCraft::Graphics::Renderable::getPipeline() {
	return this->pipeline;
}

void VulkanCraft::Graphics::Renderable::setPipeline(const std::shared_ptr<GraphicsPipeline>& pipeline) {
	this->pipeline = pipeline;
}

void VulkanCraft::Graphics::Renderable::setResourceManager(const std::shared_ptr<ResourceManager>& resourceManager) {
	this->resourceManager = resourceManager;
}
