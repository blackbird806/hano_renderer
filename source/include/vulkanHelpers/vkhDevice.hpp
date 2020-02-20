#pragma once

#include <vulkan/vulkan.hpp>

namespace hano::vkh
{
	struct Device
	{
		
		vk::Device device;
		vk::AllocationCallbacks allocator;

		vk::Queue graphicsQueue;
		vk::Queue computeQueue;
		vk::Queue transferQueue;
		vk::Queue presentQueue;
	};
}
