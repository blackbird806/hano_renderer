#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhDevice.hpp"
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct DeviceMemory
	{
		DeviceMemory(const DeviceMemory&) = delete;
		DeviceMemory& operator = (const DeviceMemory&) = delete;
		DeviceMemory& operator = (DeviceMemory&&) = delete;;
		
		DeviceMemory(DeviceMemory&&) noexcept;
		
		DeviceMemory(vkh::Device const& device, size_t size, uint32 memoryType, vk::MemoryPropertyFlags);
		~DeviceMemory();

		void* map(size_t offset, size_t size);
		void unmap();
		
		vkh::Device const& device;
		vk::DeviceMemory handle;

		private:

			uint32 findMemoryType(uint32 typeFilter, vk::MemoryPropertyFlags props) const;
	};
}
