#pragma once

#include "core/hanoException.hpp"

#define VULKAN_NON_COPIABLE(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (const ClassName&) = delete; \
	ClassName& operator = (ClassName&&) = delete;

#define VKH_CHECK(result, msg) if ((VkResult)result != VK_SUCCESS) throw ::hano::HanoException("[vulkan error] " + ::vk::to_string(result) + __FUNCTION__ + ":" + ::std::to_string(__LINE__) + " : " + msg)
