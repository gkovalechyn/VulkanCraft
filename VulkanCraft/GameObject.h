#pragma once
#include <vector>
#include "Component.h"

namespace VulkanCraft {
	namespace Core {
		class GameObject {
		public:
			GameObject();
			virtual ~GameObject();

		private:
			std::vector<Component*> components;
		};
	}
}

