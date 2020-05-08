#include <vulkanHelpers/vkhDebug.hpp>
#include <core/logger.hpp>

namespace hano::vkh
{
	VkBool32 debugVkCallback(VkDebugUtilsMessageSeverityFlagBitsEXT  messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		VkDebugUtilsMessengerCallbackDataEXT const* callbackData,
		void* userData)
	{
		hano_error("[vulkan debug callback] : {}", callbackData->pMessage);
		return VK_FALSE;
	}

	VkResult createDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerCreateInfoEXT const* createInfo,
		VkAllocationCallbacks const* allocator,
		VkDebugUtilsMessengerEXT* debugMessenger)
	{
		auto const func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func)
			return func(instance, createInfo, allocator, debugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks const* allocator)
	{
		auto const func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func)
			return func(instance, debugMessenger, allocator);
	}
		
}
