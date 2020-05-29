#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhInstance.hpp"

struct GLFWwindow;

namespace hano::vkh
{
	struct Surface
	{
		explicit Surface(Instance& inst, GLFWwindow* window, vk::AllocationCallbacks* alloc);
		~Surface();

		Instance& instance;
		vk::SurfaceKHR handle;
		vk::AllocationCallbacks* allocator;
		GLFWwindow* window;
	};
}
