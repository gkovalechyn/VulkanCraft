#pragma once
#include <vulkan/vulkan.hpp>
#include "Mesh.h"
#include "GraphicsPipeline.h"
#include "ResourceManager.h"
#include "RenderData.h"

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

			virtual const glm::mat4& getModelMatrix() = 0;

			std::shared_ptr<Mesh> getMesh();
			void setMesh(const std::shared_ptr<Mesh>& mesh);
			bool hasMesh();

			std::shared_ptr<GraphicsPipeline> getPipeline();
			void setPipeline(const std::shared_ptr<GraphicsPipeline>& pipeline);

			void setResourceManager(const std::shared_ptr<ResourceManager>& resourceManager);

			RenderData& getRenderData();
		private:
			RenderData renderData;
			std::shared_ptr<Mesh> mesh;

			std::vector<DescriptorSetData> validDescriptorSets;
			std::vector<DescriptorSetData> invalidDescriptorSets;

			bool descriptorSetsAreDirty = true;

			std::shared_ptr<GraphicsPipeline> pipeline;
			std::shared_ptr<ResourceManager> resourceManager;

			uint32_t instanceId;
		};
	}
}