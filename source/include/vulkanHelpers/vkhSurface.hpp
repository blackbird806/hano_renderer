#pragma once

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>
#include "vkhInstance.hpp"

namespace hano::vkh
{
	struct Surface
	{
		VULKAN_NON_COPIABLE(Surface);

		explicit Surface(Instance& inst, GLFWwindow* window, vk::AllocationCallbacks* alloc)
			: instance(inst), allocator(alloc)
		{
			assert(window);
			glfwCreateWindowSurface(instance.handle, window, (VkAllocationCallbacks*)&alloc, (VkSurfaceKHR*)&handle);
		}

		~Surface()
		{
			if (handle != VK_NULL_HANDLE)
			{
				instance.handle.destroySurfaceKHR(handle, allocator);
				handle = nullptr;
			}
		}
		
		Instance& instance;
		vk::SurfaceKHR handle;
		vk::AllocationCallbacks* allocator;
	};
}
