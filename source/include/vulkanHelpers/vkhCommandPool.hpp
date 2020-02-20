#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkanHelpers/vkhUtility.hpp"
#include "vkhDevice.hpp"

namespace hano::vkh
{
	struct CommandPool
	{
		VULKAN_NON_COPIABLE(CommandPool);

		CommandPool(vkh::Device const& idevice, vk::AllocationCallbacks* alloc, uint32 queueFamilyIndex, bool allowReset)
			: device(idevice), allocator(alloc)
		{
			vk::CommandPoolCreateInfo poolInfo = {};
			poolInfo.queueFamilyIndex = queueFamilyIndex;
			poolInfo.flags = allowReset ? vk::CommandPoolCreateFlagBits::eResetCommandBuffer : vk::CommandPoolCreateFlags();
			VKH_CHECK(
				device.handle.createCommandPool(&poolInfo, allocator, &handle), 
				"failed to create command Pool !");
		}
		
		~CommandPool()
		{
			if (handle)
			{
				device.handle.destroyCommandPool(handle, allocator);
				handle = nullptr;
			}
		}

		vkh::Device const& device;
		vk::CommandPool handle;
		vk::AllocationCallbacks* allocator;
	};
}
