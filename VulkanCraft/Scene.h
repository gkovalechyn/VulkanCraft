#pragma once
#include <vector>
#include "GameObject.h"
namespace VulkanCraft {
	namespace Core {
		class Scene {
		public:
			Scene();
			~Scene();

		private:
			std::vector<GameObject*> gameObjects;
		};
	}
}

