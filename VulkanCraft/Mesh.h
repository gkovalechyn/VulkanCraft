#pragma once
#include <vector>
#include "Vertex.h"
namespace VulkanCraft {
	namespace Core {
		struct Mesh {
			Mesh();
			virtual ~Mesh();
			
			std::vector<Graphics::Vertex> vertices;
		};
	}
}

