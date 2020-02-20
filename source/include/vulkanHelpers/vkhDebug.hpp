#pragma once
#include <vulkan/vulkan_core.h>

namespace hano::vkh
{
	
VKAPI_ATTR VkBool32 VKAPI_CALL debugVkCallback(VkDebugUtilsMessageSeverityFlagBitsEXT  messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	VkDebugUtilsMessengerCallbackDataEXT const* callbackData,
	void* userData);

VkResult createDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerCreateInfoEXT const* createInfo,
	VkAllocationCallbacks const* allocator,
	VkDebugUtilsMessengerEXT* debugMessenger);

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks const* pAllocator);

}
