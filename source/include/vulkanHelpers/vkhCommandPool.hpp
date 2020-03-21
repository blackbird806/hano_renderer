#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkanHelpers/vkhUtility.hpp"
#include "vkhDevice.hpp"

namespace hano::vkh
{
	struct CommandPool
	{
		CommandPool(vkh::Device const& device_, uint32 queueFamilyIndex, bool allowReset);

		vkh::Device const* device;
		vk::UniqueCommandPool handle;
	};
}
