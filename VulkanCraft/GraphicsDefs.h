#pragma once
#include <vulkan/vulkan.hpp>

typedef struct ImageView {
	vk::Image image;
	vk::ImageView view;
} ImageView;