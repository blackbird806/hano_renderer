#include <vulkanHelpers/vkhSurface.hpp>
#include <glfw/glfw3.h>

using namespace hano::vkh;

Surface::Surface(Instance& inst, GLFWwindow* window, vk::AllocationCallbacks* alloc): instance(inst), allocator(alloc)
{
	assert(window);
	VkSurfaceKHR tmpSurface;
	glfwCreateWindowSurface(instance.handle, window, reinterpret_cast<VkAllocationCallbacks*>(allocator), &tmpSurface);
	handle = tmpSurface;
}

Surface::~Surface()
{
	if (handle)
	{
		instance.handle.destroySurfaceKHR(handle, allocator);
		handle = nullptr;
	}
}
