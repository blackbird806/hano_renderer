#pragma once

#include <vulkan/vulkan.hpp>
#include <core/hanoConfig.hpp>

namespace hano
{
	struct RenderInfo
	{
		vk::CommandBuffer commandBuffer;
		uint32 currentFrame;
	};
}