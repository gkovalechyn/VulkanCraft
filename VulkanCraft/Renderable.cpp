#include "Renderable.h"

uint32_t VulkanCraft::Graphics::Renderable::getNumberOfVerticesToDraw() {
	if (this->mesh) {
		return static_cast<uint32_t>(this->mesh->getVertexCount());
	} else {
		return 0;
	}
}

uint32_t VulkanCraft::Graphics::Renderable::getInstanceNumber() {
	return this->instanceId;
}

uint32_t VulkanCraft::Graphics::Renderable::getVertexOffset() {
	return 0;
}

bool VulkanCraft::Graphics::Renderable::areDescriptorSetsDirty() {
	return this->descriptorSetIsDirty;
}

vk::Buffer VulkanCraft::Graphics::Renderable::getIndexBuffer() {
	return this->mesh->getIndexBuffer();
}

uint32_t VulkanCraft::Graphics::Renderable::getIndexBufferOffset() {
	return 0;
}

vk::Buffer VulkanCraft::Graphics::Renderable::getVertexBuffer() {
	return this->mesh->getVertexBuffer();
}

void VulkanCraft::Graphics::Renderable::setMesh(Core::Mesh * mesh) {
	this->mesh = mesh;
}
