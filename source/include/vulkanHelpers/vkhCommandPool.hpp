#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkanHelpers/vkhUtility.hpp"
#include "vkhDevice.hpp"

namespace hano::vkh
{
	struct CommandPool
	{
		VULKAN_NON_COPIABLE_NON_MOVABLE(CommandPool);

		CommandPool(vkh::Device const& idevice, uint32 queueFamilyIndex, bool allowReset)
			: device(idevice)
		{
			vk::CommandPoolCreateInfo poolInfo = {};
			poolInfo.queueFamilyIndex = queueFamilyIndex;
			poolInfo.flags = allowReset ? vk::CommandPoolCreateFlagBits::eResetCommandBuffer : vk::CommandPoolCreateFlags();
			VKH_CHECK(
				device.handle.createCommandPool(&poolInfo, device.allocator, &handle),
				"failed to create command Pool !");
		}
		
		~CommandPool()
		{
			if (handle)
			{
				device.handle.destroyCommandPool(handle, device.allocator);
				handle = nullptr;
			}
		}

		vkh::Device const& device;
		vk::CommandPool handle;
	};
}
