#pragma once
#include <vector>
#include "Vertex.h"
#include "libs/tiny_obj_loader.h"
#include "GPUAllocation.h"
#include "Logger.h"

namespace VulkanCraft {
	namespace Graphics {
		class Mesh {
		public:
			Mesh(const std::vector<Graphics::Vertex>& vertices, const std::vector<uint32_t>& indices);
			Mesh(const tinyobj::attrib_t& attributes, const tinyobj::shape_t& shape);
			virtual ~Mesh();
			
			static std::vector<std::unique_ptr<Mesh>> fromOBJ(const std::string& path);

			Mesh(const Mesh& other) = delete;

			/// <summary>
			/// Returns how many vertices are in this mesh.
			/// </summary>
			/// <returns>The number of vertices in the mesh</returns>
			size_t getVertexCount() const noexcept;

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

			void setVertexBuffer(GPUAllocation vertexAllocation) noexcept;
			void setIndexBuffer(GPUAllocation indexAllocation) noexcept;

			GPUAllocation getVertexBuffer() const noexcept;
			GPUAllocation getIndexBuffer() const noexcept;

			size_t getIndexCount() const noexcept;

			const std::vector<Vertex>& getVertices();
			const std::vector<uint32_t>& getIndices();

		private:
			std::vector<Graphics::Vertex> vertices;
			std::vector<uint32_t> indices;

			GPUAllocation vertexBuffer;
			GPUAllocation indexBuffer;

			bool dirty;
		};
	}
}

