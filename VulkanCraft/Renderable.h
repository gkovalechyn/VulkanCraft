#pragma once
#include <vulkan/vulkan.hpp>
#include "Mesh.h"
#include "GraphicsPipeline.h"
#include "ResourceManager.h"

namespace VulkanCraft {
	namespace Graphics {
		class Renderable {
		public:
			virtual ~Renderable() {};

			/// <summary>
			/// Gets the instance number of this object.
			/// </summary>
			/// <returns></returns>
			virtual uint32_t getInstanceNumber();

			std::vector<DescriptorSetData>& getDescriptorSets();
			bool areDescriptorSetsDirty();

			template <typename T>
			void setShaderBufferData(const uint32_t set, const uint32_t binding, T value, uint64_t offsetInBuffer = 0);

			virtual const glm::mat4& getModelMatrix() = 0;

			std::shared_ptr<Mesh> getMesh();
			void setMesh(const std::shared_ptr<Mesh>& mesh);
			bool hasMesh();

			std::shared_ptr<GraphicsPipeline> getPipeline();
			void setPipeline(const std::shared_ptr<GraphicsPipeline>& pipeline);

			void setResourceManager(const std::shared_ptr<ResourceManager>& resourceManager);
		private:
			std::shared_ptr<Mesh> mesh;

			std::vector<DescriptorSetData> validDescriptorSets;
			std::vector<DescriptorSetData> invalidDescriptorSets;

			bool descriptorSetsAreDirty = true;

			std::shared_ptr<GraphicsPipeline> pipeline;
			std::shared_ptr<ResourceManager> resourceManager;

			uint32_t instanceId;
		};

		template<typename T>
		inline void Renderable::setShaderBufferData(const uint32_t set, const uint32_t binding, T value, uint64_t offsetInBuffer) {
			const auto descriptorData = this->pipeline->allocateDescriptorSet(set, binding);
			vk::DescriptorBufferInfo bufferInfo;
			bufferInfo
				.setBuffer(descriptorData.data.buffer)
				.setOffset(offsetInBuffer)
				.setRange(sizeof(T));
		}
	}
}