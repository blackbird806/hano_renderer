#pragma once
#include <core/hanoConfig.hpp>

namespace hano::vkh
{
#if HANO_RENDERER_ENABLE_RAYTRACING
	void load_VK_NV_ray_tracing(VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr) noexcept;
#endif
}