#pragma once
#include <vulkan/vulkan.hpp>

struct ImageView {
	vk::Image image;
	vk::ImageView view;
};