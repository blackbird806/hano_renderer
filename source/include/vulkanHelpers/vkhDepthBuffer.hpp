#pragma once
#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"
#include "vkhDeviceMemory.hpp"
#include "vkhImageView.hpp"
#include "vkhImage.hpp"

namespace hano::vkh
{
	struct CommandPool;
	
	struct DepthBuffer
	{
		DepthBuffer(CommandPool& commandPool, vk::Extent2D extent);
		
		HANO_NODISCARD static bool hasStencilComponent(vk::Format format) noexcept;

		vkh::Image image;
		vkh::ImageView imageView;
	};
}
