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

		Image();
		Image(vkh::Device const& idevice, vk::Extent2D ext, vk::Format fmt,
			vk::MemoryPropertyFlags memoryPropertyFlags,
			vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			vk::ImageUsageFlags usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
		
		void init(vkh::Device const& idevice, vk::Extent2D ext, vk::Format fmt,
			vk::MemoryPropertyFlags memoryPropertyFlags,
			vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			vk::ImageUsageFlags usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);

		void destroy();

		void transitionImageLayout(CommandPool& commandPool, vk::ImageLayout newLayout);
		void copyFrom(CommandPool& commandPool, vkh::Buffer const& buffer);
		void insertBarrier(vk::CommandBuffer commandBuffer, vk::ImageSubresourceRange subresourceRange,
			vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask, vk::ImageLayout newLayout);

		vk::UniqueImage handle;
		vk::UniqueDeviceMemory memory;

		vkh::Device const* device;
		vk::Extent2D extent;
		vk::Format format;
		vk::ImageLayout imageLayout;
	};
}
