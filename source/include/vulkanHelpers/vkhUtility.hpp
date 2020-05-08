#pragma once

#include "core/hanoException.hpp"

#define VKH_CHECK(result, msg) if ((VkResult)result != VK_SUCCESS) throw ::hano::HanoException("[vulkan error] " + ::vk::to_string(result) + __FUNCTION__ + ":" + ::std::to_string(__LINE__) + " : " + msg)
