#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>
#include <unordered_map>
#include <core/hanoException.hpp>
#include "vkhDescriptorBinding.hpp"
#include "vkhDescriptorPool.hpp"
#include "vkhDescriptorSetLayout.hpp"
#include "vkhDescriptorSets.hpp"

namespace hano::vkh
{
	struct Device;

	struct DescriptorSetManager
	{
		DescriptorSetManager(Device const& device, std::vector<DescriptorBinding> const& descriptorBindings, size_t maxSets)
		{
			// Sanity check to avoid binding different resources to the same binding point.
			std::unordered_map<uint32_t, vk::DescriptorType> bindingTypes;

			for (auto const& binding : descriptorBindings)
			{
				if (!bindingTypes.insert(std::make_pair(binding.binding, binding.type)).second)
				{
					throw HanoException("binding collision");
				}
			}

			descriptorPool = std::make_unique<DescriptorPool>(device, descriptorBindings, maxSets);
			descriptorSetLayout = std::make_unique<DescriptorSetLayout>(device, descriptorBindings);
			descriptorSets = std::make_unique<DescriptorSets>(*descriptorPool, *descriptorSetLayout, bindingTypes, maxSets);
		}

		std::unique_ptr<vkh::DescriptorSetLayout> descriptorSetLayout;
		std::unique_ptr<vkh::DescriptorPool> descriptorPool;
		std::unique_ptr<vkh::DescriptorSets> descriptorSets;
	};

}