#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhDescriptorBinding.hpp"

namespace hano::vkh
{
	struct Device;

	struct DescriptorSetLayout
	{
		DescriptorSetLayout(Device const& device, std::vector<DescriptorBinding> const& descriptorBindings);

		vkh::Device const* device;
		vk::UniqueDescriptorSetLayout handle;
	};
}