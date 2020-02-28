#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct ImageView;
	struct RenderPass;

	struct FrameBuffer
	{
		explicit FrameBuffer(vkh::ImageView const&, vkh::RenderPass const&);

		vkh::ImageView const* imageView;
		vkh::RenderPass const* renderPass;
		vk::UniqueFramebuffer handle;
	};
}