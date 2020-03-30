#pragma once

#include <array>
#include <vulkan/vulkan_core.h>

namespace hano
{
	using uint = unsigned int;
	using uint64 = uint64_t;
	using uint32 = uint32_t;
	using uint16 = uint16_t;
	using uint8 = uint8_t;

	using int64 = int64_t;
	using int32 = int32_t;
	using int16 = int16_t;
	using int8 = int8_t;

	using byte = uint8;
	using uchar = unsigned char;
}

#if (defined _MSC_VER && defined _DEBUG) || (defined __GNUG__ && defined DEBUG)
#define HANO_RENDERER_DEBUG 1
#else
#define HANO_RENDERER_DEBUG 0
#endif //TODO other compiler support

#if (defined VK_NV_ray_tracing) || (defined HANO_RENDERER_FORCE_ENABLE_RAYTRACING)
#define HANO_RENDERER_ENABLE_RAYTRACING 1
#else
#define HANO_RENDERER_ENABLE_RAYTRACING 0
#error "non rtx gpu aren not supported yet !"
#endif

#define HANO_FORCE_NODISCARD
#if __has_cpp_attribute(nodiscard) || (defined HANO_FORCE_NODISCARD)
#define HANO_NODISCARD [[nodiscard]]
#else
#define HANO_NODISCARD 
#endif

#define HANO_FORCE_ENABLE_CPP_CONCEPTS

#if (__cpp_concepts && __cpp_lib_concepts) || defined(HANO_FORCE_ENABLE_CPP_CONCEPTS)
#define HANO_RENDERER_USE_CONCEPTS 1
#else
#define HANO_RENDERER_USE_CONCEPTS 0
#endif

namespace hano
{
	constexpr const char* c_engineName = "hano_renderer";

	std::array constexpr c_vulkanValidationLayers = { "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_standard_validation" };
	std::array constexpr c_vulkanDefaultDeviceRequiredExtentions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_NV_RAY_TRACING_EXTENSION_NAME, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME };

#if HANO_RENDERER_DEBUG
	inline bool constexpr c_vulkanEnableValidationsLayers = true;
#else
	inline bool constexpr c_vulkanEnableValidationsLayers = false;
#endif // _DEBUG
}
