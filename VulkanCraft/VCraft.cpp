#include "VCraft.h"

using namespace VulkanCraft;
using namespace VulkanCraft::Core;


VCraft::VCraft() {
}


VCraft::~VCraft() {
}

Graphics::RenderingEngine& VulkanCraft::Core::VCraft::getRenderingEngine() noexcept{
	return *this->renderingEngine;
}
