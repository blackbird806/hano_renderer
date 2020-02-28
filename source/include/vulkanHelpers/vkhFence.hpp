#pragma once

#include <vulkan/vulkan.hpp>
#include <core/hanoConfig.hpp>

namespace hano::vkh
{
	struct Device;

	struct Fence
	{
		explicit Fence(vkh::Device const&, bool signal);

		void reset();
		void wait(uint64 timeout);

		vkh::Device const* device;
		vk::UniqueFence handle;
	};
}