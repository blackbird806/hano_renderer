#pragma once
#include <type_traits>
#include <vulkan/vulkan.hpp>
#include <core/hanoConfig.hpp>

namespace hano::vkh
{
	// https://stackoverflow.com/a/41495065
	template <typename>
	struct isUniqueHandle : public std::false_type
	{ };

	template <typename T, typename Dispatch>
	struct isUniqueHandle<vk::UniqueHandle<T, Dispatch>> : public std::true_type
	{ };
	
	VKAPI_ATTR VkBool32 VKAPI_CALL debugVkCallback(VkDebugUtilsMessageSeverityFlagBitsEXT  messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		VkDebugUtilsMessengerCallbackDataEXT const* callbackData,
		void* userData);

	VkResult createDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerCreateInfoEXT const* createInfo,
		VkAllocationCallbacks const* allocator,
		VkDebugUtilsMessengerEXT* debugMessenger);

	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks const* pAllocator);


	template<typename T>
	void setObjectName(T const& object, const char* name, vk::Device device)
	{
#if HANO_RENDERER_DEBUG
		if constexpr (isUniqueHandle<T>::value)
			device.setDebugUtilsObjectNameEXT(vk::DebugUtilsObjectNameInfoEXT{ T::element_type::objectType, reinterpret_cast<const uint64_t&>(object.get()), name });
		else
			device.setDebugUtilsObjectNameEXT(vk::DebugUtilsObjectNameInfoEXT{ T::objectType, reinterpret_cast<const uint64_t&>(object), name });
#endif
	}
}
