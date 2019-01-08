#pragma once
#include <vector>
#include "Vertex.h"
#include "libs/tiny_obj_loader.h"
#include "libs/vk_mem_alloc.h"

namespace VulkanCraft {
	namespace Core {
		struct Mesh {
			Mesh(std::vector<Graphics::Vertex> vertices);
			Mesh(const tinyobj::attrib_t& attributes, const tinyobj::shape_t& shape);
			virtual ~Mesh();
			
			Mesh(const Mesh& other) = delete;

			/// <summary>
			/// Returns how many vertices are in this mesh.
			/// </summary>
			/// <returns>The number of vertices in the mesh</returns>
			int getVertexCount();

			/// <summary>
			/// Sets the vertex at the given index to a copy of the given vertex.
			/// </summary>
			/// <param name="index">The index.</param>
			/// <param name="vertex">The vertex.</param>
			void setVertex(int index, Graphics::Vertex& vertex);

			/// <summary>
			/// Adds a copy of the given vertex at the end of the vertex list.
			/// </summary>
			/// <param name="vertex">The vertex.</param>
			void addVertex(Graphics::Vertex& vertex);

			/// <summary>
			/// Gets the vertex at the given index.
			/// </summary>
			/// <param name="index">The index.</param>
			/// <returns>A pointer to the vertex at the given index</returns>
			Graphics::Vertex* getVertex(int index);

			vk::Buffer getVertexBuffer();
			vk::Buffer getIndexBuffer();
		private:
			std::vector<Graphics::Vertex> vertices;
			std::vector<uint32_t> indices;

			vk::Buffer vertexBuffer;
			vk::Buffer indexBuffer;

			bool dirty;

			VmaAllocation vertexAllocation;
			VmaAllocation indexAllocation;
		};
	}
}

