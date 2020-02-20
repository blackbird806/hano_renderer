#include <vulkanHelpers/vkhSurface.hpp>
#include <glfw/glfw3.h>

hano::vkh::Surface::
Surface(Instance& inst, GLFWwindow* window, vk::AllocationCallbacks* alloc): instance(inst), allocator(alloc)
{
	assert(window);
	VkAllocationCallbacks* tmp = nullptr;
	VkSurfaceKHR tm;
	glfwCreateWindowSurface(instance.handle, window, (VkAllocationCallbacks*)tmp, (VkSurfaceKHR*)&tm);
	handle = tm;
}

hano::vkh::Surface::~Surface()
{
	if (handle)
	{
		instance.handle.destroySurfaceKHR(handle, allocator);
		handle = nullptr;
	}
}
