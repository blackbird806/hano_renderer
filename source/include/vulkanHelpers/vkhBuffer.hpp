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
		Buffer();
		Buffer(vkh::Device const& idevice, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props);

		void create(vkh::Device const& idevice, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props);
		void destroy();

		operator bool() const noexcept;

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
