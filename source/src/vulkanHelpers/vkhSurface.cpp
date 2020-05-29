#include <vulkanHelpers/vkhSurface.hpp>
#include <glfw/glfw3.h>

using namespace hano::vkh;

Surface::Surface(Instance& inst, GLFWwindow* window_, vk::AllocationCallbacks* alloc): instance(inst), allocator(alloc), window(window_)
{
	assert(window);
	VkSurfaceKHR tmpSurface;
	glfwCreateWindowSurface(instance.handle.get(), window, reinterpret_cast<VkAllocationCallbacks*>(allocator), &tmpSurface);
	handle = tmpSurface;
}

Surface::~Surface()
{
	if (handle)
	{
		instance.handle.get().destroySurfaceKHR(handle, allocator);
		handle = nullptr;
	}
}
