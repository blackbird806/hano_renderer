#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"

namespace hano::vkh
{
	struct Device;

	struct Semaphore
	{
		explicit Semaphore(Device const& idevice);

		Device const* device;
		vk::UniqueSemaphore handle;
	};
}