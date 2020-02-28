#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct Device;
	struct CommandPool;
	struct Buffer;
	
	struct Image
	{
		Image(const Image&) = delete;
		Image& operator = (const Image&) = delete;

		Image(vkh::Device const& idevice, vk::Extent2D ext, vk::Format fmt,
			vk::MemoryPropertyFlags memoryPropertyFlags,
			vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			vk::ImageUsageFlags usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
		
		~Image();

		void transitionImageLayout(CommandPool& commandPool, vk::ImageLayout newLayout);
		void copyFrom(CommandPool& commandPool, vkh::Buffer const& buffer);
		
		vk::Image handle;
		vk::DeviceMemory memory;

		vkh::Device const& device;
		vk::Extent2D const extent;
		vk::Format const format;
		vk::ImageLayout imageLayout;
	};
}
