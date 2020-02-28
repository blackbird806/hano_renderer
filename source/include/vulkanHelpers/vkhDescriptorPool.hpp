#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhDescriptorBinding.hpp"

namespace hano::vkh
{
	struct Device;
	
	struct DescriptorPool
	{
		DescriptorPool(Device const& device, std::vector<DescriptorBinding> const& descriptorBindings, size_t maxSets);

		Device const* device;
		vk::UniqueDescriptorPool handle;
	};

}