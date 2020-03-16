#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhDescriptorBinding.hpp"

namespace hano::vkh
{
	struct Device;

	struct DescriptorSetLayout
	{
		DescriptorSetLayout();
		DescriptorSetLayout(Device const& device, std::vector<DescriptorBinding> const& descriptorBindings_);
		
		void init(Device const& device, std::vector<DescriptorBinding> const& descriptorBindings_);
		void destroy();

		std::vector<DescriptorBinding> const& getDescriptorBindings() const noexcept;

		vkh::Device const* device;
		vk::UniqueDescriptorSetLayout handle;

		private:
			std::vector<DescriptorBinding> m_descriptorBindings;
	};
}