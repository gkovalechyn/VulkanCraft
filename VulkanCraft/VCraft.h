#pragma once
#include "RenderingEngine.h"

namespace VulkanCraft {
	namespace Core {
		class VCraft {
		public:
			VCraft();
			~VCraft();

			VCraft(const VCraft& other) = delete;
			VCraft(VCraft&& other) = delete;

			Graphics::RenderingEngine& getRenderingEngine() noexcept;

		private:
			std::unique_ptr<Graphics::RenderingEngine> renderingEngine;
		};
	}
}

