#pragma once
#include <vulkan/vulkan.hpp>
#include "Mesh.h"

namespace VulkanCraft {
	namespace Graphics {
		class Renderable {
		public:
			virtual ~Renderable() {};

			/// <summary>
			/// Gets the number of vertices to draw.
			/// </summary>
			/// <returns>How many vertices should be drawn</returns>
			virtual uint32_t getNumberOfVerticesToDraw();

			/// <summary>
			/// Gets the instance number of this object.
			/// </summary>
			/// <returns></returns>
			virtual uint32_t getInstanceNumber();

			/// <summary>
			/// Gets the offset of this object's vertices in this object's vertex buffer.
			/// </summary>
			/// <returns>The offset of this object's vertices in the vertex buffer</returns>
			virtual uint32_t getVertexOffset();

			virtual vk::DescriptorSet getDescriptorSet();
			virtual bool isDescriptorSetDirty();

			/// <summary>
			/// Gets the index buffer.
			/// </summary>
			/// <returns></returns>
			virtual vk::Buffer getIndexBuffer();

			/// <summary>
			/// Gets the index buffer offset.
			/// </summary>
			/// <returns>The offset within the index buffer where the indices for this object start</returns>
			virtual uint32_t getIndexBufferOffset();
			virtual vk::Buffer getVertexBuffer();

			void setMesh(Core::Mesh* mesh);
		private:
			Core::Mesh* mesh;
			vk::DescriptorSet descriptorSet;
			bool descriptorSetIsDirty = true;
			uint32_t instanceId;
		};
	}
}

