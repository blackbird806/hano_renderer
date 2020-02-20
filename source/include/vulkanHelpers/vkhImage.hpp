#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct Image
	{
		vk::Device device;
		vk::Image image;
		vk::DeviceMemory memory;

		void create(vk::ImageCreateInfo const& imageInfo, vk::AllocationCallbacks allocator, vk::MemoryPropertyFlags props)
		{
			VKH_CHECK(device.createImage(&imageInfo, &allocator, &image), "failed to create Image !");

			VkMemoryRequirements memRequirements = device.getImageMemoryRequirements(image, &memRequirements);
		}
	};
}
