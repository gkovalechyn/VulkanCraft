#include "TestRenderable.h"

VulkanCraft::TestRenderable::TestRenderable() {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "Resources/Models/Cube/Export/cube.obj");

	if (!warn.empty()) {
		Core::Logger::warning(warn);
	}

	if (!err.empty()) {
		Core::Logger::error(err);
		throw std::runtime_error(err);
	}

	Core::Logger::debug("Loaded cube obj");
	this->setMesh(new Core::Mesh(attrib, shapes[0]));
	
	Core::Logger::debug("Cube mesh created");
}


VulkanCraft::TestRenderable::~TestRenderable() {

}
