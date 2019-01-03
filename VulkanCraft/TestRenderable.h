#pragma once
#include "Renderable.h"
#include "libs/tiny_obj_loader.h"

namespace VulkanCraft {
	class TestRenderable : public Graphics::Renderable {
	public:
		TestRenderable();
		~TestRenderable();

		// Inherited via Renderable
		virtual void writeRenderCommands(const vk::CommandBuffer & commandBuffer) override;
	};
}

