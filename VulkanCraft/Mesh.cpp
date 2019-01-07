#include "Mesh.h"

using namespace VulkanCraft;

VulkanCraft::Core::Mesh::Mesh(std::vector<Graphics::Vertex> vertices) : vertices(vertices){
}

VulkanCraft::Core::Mesh::~Mesh() {
}

int VulkanCraft::Core::Mesh::getVertexCount() {
	return this->vertices.size();
}

void VulkanCraft::Core::Mesh::setVertex(int index, Graphics::Vertex & vertex) {
	if (index >= 0 && index < this->vertices.size()) {
		this->vertices[index] = vertex;
	}
}

void VulkanCraft::Core::Mesh::addVertex(Graphics::Vertex & vertex) {
	this->vertices.push_back(vertex);
}

Graphics::Vertex* VulkanCraft::Core::Mesh::getVertex(int index) {
	if (index >= 0 && index < this->vertices.size()) {
		return &this->vertices[index];
	}
}
