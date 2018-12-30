#pragma once
#include "Renderable.h"
namespace VulkanCraft {
	class TestRenderable : public Graphics::Renderable {
	public:
		TestRenderable();
		~TestRenderable();

		// Inherited via Renderable
		virtual void writeRenderCommands(const vk::CommandBuffer & commandBuffer) override;
	};
}

