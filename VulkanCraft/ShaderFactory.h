#pragma once
#include <vulkan/vulkan.hpp>
class ShaderFactory {
public:
	ShaderFactory(const vk::Device& device) : device(device) {};
	~ShaderFactory();
private:
	vk::Device device;
};

