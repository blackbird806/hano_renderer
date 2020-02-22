#pragma once

#include "core/hanoException.hpp"

#define VULKAN_NON_COPIABLE(ClassName) \
ClassName(const ClassName&) = delete; \
ClassName& operator = (const ClassName&) = delete; \

#define VULKAN_NON_COPIABLE_NON_MOVABLE(ClassName) \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (ClassName&&) = delete; \
	VULKAN_NON_COPIABLE(ClassName)

#define VKH_CHECK(result, msg) if ((VkResult)result != VK_SUCCESS) throw ::hano::HanoException("[vulkan error] " + ::vk::to_string(result) + __FUNCTION__ + ":" + ::std::to_string(__LINE__) + " : " + msg)
