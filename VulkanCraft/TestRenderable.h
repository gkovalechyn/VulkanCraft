#pragma once
#include "Renderable.h"
#include "Mesh.h"
#include "libs/tiny_obj_loader.h"
#include "Logger.h"

namespace VulkanCraft {
	class TestRenderable : public Graphics::Renderable {
	public:
		TestRenderable();
		~TestRenderable();
	};
}

