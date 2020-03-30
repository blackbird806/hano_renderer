#pragma once 
#include <core/hanoConfig.hpp>

namespace hano
{
	void load_VK_EXT_debug_utils(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr) noexcept;
}