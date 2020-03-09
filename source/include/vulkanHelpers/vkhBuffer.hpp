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

		void* map(vk::DeviceSize offset = 0);
		void  unMap();

		void copyFrom(CommandPool& commandPool, Buffer const& buffer, vk::DeviceSize size);
		
		vkh::Device const* device;
		vk::UniqueBuffer handle;
		vk::UniqueDeviceMemory memory;

		private:

		vk::DeviceSize m_size;
	};
}
