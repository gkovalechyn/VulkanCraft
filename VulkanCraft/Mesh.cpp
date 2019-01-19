#include "Mesh.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Graphics;

VulkanCraft::Graphics::Mesh::Mesh(std::vector<Graphics::Vertex> vertices) : vertices(vertices){
}

VulkanCraft::Graphics::Mesh::Mesh(const tinyobj::attrib_t & attributes, const tinyobj::shape_t& shape) {
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

VulkanCraft::Graphics::Mesh::~Mesh() {
}

std::vector<std::unique_ptr<Mesh>> VulkanCraft::Graphics::Mesh::fromOBJ(const std::string& path) {
	std::vector<std::unique_ptr<Mesh>> meshes;

	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, path.c_str());

	if (!warn.empty()) {
		Core::Logger::warning(warn);
	}

	if (!err.empty()) {
		Core::Logger::error(err);
		throw std::runtime_error(err);
	}

	for (const auto& shape : shapes) {
		meshes.push_back(std::make_unique<Mesh>(attributes, shape));
	}

	return meshes;
}

size_t VulkanCraft::Graphics::Mesh::getVertexCount() const noexcept{
	return this->vertices.size();
}

void VulkanCraft::Graphics::Mesh::setVertex(int index, Graphics::Vertex & vertex) {
	if (index >= 0 && index < this->vertices.size()) {
		this->vertices[index] = vertex;
		this->dirty = true;
	}
}

void VulkanCraft::Graphics::Mesh::addVertex(Graphics::Vertex & vertex) {
	this->vertices.push_back(vertex);
	this->dirty = true;
}

Graphics::Vertex* VulkanCraft::Graphics::Mesh::getVertex(int index) {
	if (index >= 0 && index < this->vertices.size()) {
		return &this->vertices[index];
	}else{
		throw std::runtime_error("Invalid vertex index, index  < 0 or index > vertices.size()");
	}
}

void VulkanCraft::Graphics::Mesh::setVertexBuffer(VmaAllocation allocation, vk::Buffer & buffer) noexcept {
	this->vertexAllocation = allocation;
	this->vertexBuffer = buffer;
}

void VulkanCraft::Graphics::Mesh::setIndexBuffer(VmaAllocation allocation, vk::Buffer & buffer) noexcept {
	this->indexAllocation = allocation;
	this->indexBuffer = buffer;
}

vk::Buffer VulkanCraft::Graphics::Mesh::getVertexBuffer() const noexcept {
	return this->vertexBuffer;
}

VmaAllocation VulkanCraft::Graphics::Mesh::getVertexBufferAllocation() const noexcept {
	return this->vertexAllocation;
}

vk::Buffer VulkanCraft::Graphics::Mesh::getIndexBuffer() const noexcept {
	return this->indexBuffer;
}

VmaAllocation VulkanCraft::Graphics::Mesh::getIndexBufferAllocation() const noexcept {
	return this->indexAllocation;
}

size_t VulkanCraft::Graphics::Mesh::getIndexCount() const noexcept {
	return this->indices.size();
}

const std::vector<Vertex>& VulkanCraft::Graphics::Mesh::getVertices() {
	return this->vertices;
}

const std::vector<uint32_t>& VulkanCraft::Graphics::Mesh::getIndices() {
	return this->indices;
}
