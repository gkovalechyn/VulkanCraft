#pragma once
#include <vector>
#include "Vertex.h"
#include "libs/vk_mem_alloc.h"

namespace VulkanCraft {
	namespace Core {
		struct Mesh {
			Mesh(std::vector<Graphics::Vertex> vertices);
			Mesh(const tinyobj::attrib_t& attributes);
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

		private:
			std::vector<Graphics::Vertex> vertices;

			/// <summary>
			/// The GPU memory buffer.
			/// </summary>
			vk::Buffer buffer;

			/// <summary>
			/// Wether the local buffer is dirty and needs to be re-sent to the GPU.
			/// </summary>
			bool dirty;

			VmaAllocation* allocation;
		};
	}
}

