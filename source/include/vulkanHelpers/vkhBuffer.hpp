#pragma once

#include <vulkan/vulkan.hpp>
#include <span>
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct Device;
	struct DeviceMemory;
	struct CommandPool;
	
	struct Buffer
	{
		Buffer();
		Buffer(vkh::Device const& device_, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props);

		void init(vkh::Device const& device_, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props);
		void destroy();

		operator bool() const noexcept;

		template<typename T>
		void setDataArray(std::span<T> data)
		{
			assert(data.size_bytes() == getSize());
			void* bufferData = map();
				memcpy(bufferData, data.data(), getSize());
			unMap();
		}

		template<typename T>
		void setData(T data)
		{
			assert(sizeof(T) == getSize());
			void* bufferData = map();
				memcpy(bufferData, &data, getSize());
			unMap();
		}

		void* map(vk::DeviceSize offset = 0);
		void  unMap();

		vk::DeviceSize getSize() const noexcept;

		void copyFrom(CommandPool& commandPool, Buffer const& buffer, vk::DeviceSize size);
		
		vkh::Device const* device;
		vk::UniqueBuffer handle;
		vk::UniqueDeviceMemory memory;

		private:

		vk::DeviceSize m_size;
	};
}
