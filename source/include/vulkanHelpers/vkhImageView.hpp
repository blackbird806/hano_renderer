#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct Device;
	
	struct ImageView
	{
		VULKAN_NON_COPIABLE(ImageView);
		
		explicit ImageView(Device const& idevice, vk::Image img, vk::Format fmt, vk::ImageAspectFlagBits aspectFlags);
		~ImageView();
		
		vk::ImageView handle;
		vkh::Device const& device;
		vk::Image image;
	};
}
