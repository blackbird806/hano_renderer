#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct Device;
	
	struct ImageView
	{
		ImageView();
		ImageView(ImageView&& other) noexcept = default;
		ImageView& operator=(ImageView&& other) = default;
		
		explicit ImageView(Device const& device_, vk::Image img, vk::Format fmt, vk::ImageAspectFlagBits aspectFlags);

		void init(Device const& device_, vk::Image img, vk::Format fmt, vk::ImageAspectFlagBits aspectFlags);
		void destroy();

		vk::UniqueImageView handle;
		vkh::Device const* device;
		vk::Image image;
	};
}
