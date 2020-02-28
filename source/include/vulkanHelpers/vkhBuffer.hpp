#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct Device;
	struct DeviceMemory;
	struct CommandPool;
	
	struct Buffer
	{
		VULKAN_NON_COPIABLE_NON_MOVABLE(Buffer);

		Buffer(vkh::Device const& idevice, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props);
		~Buffer();

		void copyFrom(CommandPool& commandPool, Buffer const& buffer, vk::DeviceSize);
		
		vkh::Device const& device;
		vk::Buffer handle;
		vk::DeviceMemory memory;
	};
}
