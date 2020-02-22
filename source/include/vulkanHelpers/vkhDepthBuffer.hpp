#pragma once
#include <optional>
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
		VULKAN_NON_COPIABLE_NON_MOVABLE(DepthBuffer);

		DepthBuffer(CommandPool& commandPool, vk::Extent2D extent);
		~DepthBuffer();
		
		HANO_NODISCARD static bool hasStencilComponent(vk::Format format) noexcept;

		// @TODO clean this
		// using optional to control init / destruction order
		std::optional<vkh::Image> image;
		std::optional<vkh::DeviceMemory> imageMemory;
		std::optional<vkh::ImageView> imageView;
	};
}
