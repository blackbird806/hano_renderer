#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhInstance.hpp"

namespace hano::vkh
{
	struct Surface
	{
		VULKAN_NON_COPIABLE_NON_MOVABLE(Surface);

		explicit Surface(Instance& inst, GLFWwindow* window, vk::AllocationCallbacks* alloc);
		~Surface();
		
		Instance& instance;
		vk::SurfaceKHR handle;
		vk::AllocationCallbacks* allocator;
	};
}
