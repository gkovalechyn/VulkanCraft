#include "Mesh.h"

using namespace VulkanCraft;

VulkanCraft::Core::Mesh::Mesh(std::vector<Graphics::Vertex> vertices) : vertices(vertices){
}

VulkanCraft::Core::Mesh::Mesh(const tinyobj::attrib_t & attributes, const tinyobj::shape_t& shape) {
	uint32_t i = 0;
	for (const auto& index : shape.mesh.indices) {
		Graphics::Vertex vertex;

		vertex.position = {
			attributes.vertices[index.vertex_index * 3 + 0],
			attributes.vertices[index.vertex_index * 3 + 1],
			attributes.vertices[index.vertex_index * 3 + 2]
		};

		vertex.normal = {
			attributes.normals[index.normal_index * 3 + 0],
			attributes.normals[index.normal_index * 3 + 1],
			attributes.normals[index.normal_index * 3 + 2]
		};

		vertex.uv = {
			attributes.texcoords[index.texcoord_index * 2 + 0],
			attributes.texcoords[index.texcoord_index * 2 + 1]
		};

		vertex.color = {1.0, 1.0f, 1.0f};

		this->vertices.push_back(vertex);
		this->indices.push_back(i);
		i++;
	}
}

VulkanCraft::Core::Mesh::~Mesh() {
}

int VulkanCraft::Core::Mesh::getVertexCount() {
	return static_cast<int>(this->vertices.size());
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
	}else{
		throw std::runtime_error("Invalid vertex index, index  < 0 or index > vertices.size()");
	}
}

vk::Buffer VulkanCraft::Core::Mesh::getVertexBuffer() {
	return this->vertexBuffer;
}

vk::Buffer VulkanCraft::Core::Mesh::getIndexBuffer() {
	return this->indexBuffer;
}
