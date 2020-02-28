#pragma once

#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct Swapchain;
	struct DepthBuffer;

	struct RenderPass
	{
		VULKAN_NON_COPIABLE(RenderPass);

		RenderPass(vkh::Swapchain const& iswapChain, vkh::DepthBuffer const& idepthBuffer, bool clearColorBuffer, bool clearDepthBuffer);

		vkh::Swapchain const* swapchain;
		vkh::DepthBuffer const* depthBuffer;
		vk::UniqueRenderPass handle;
	};
}