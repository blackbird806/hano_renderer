#pragma once

#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "vkhDescriptorBinding.hpp"

namespace hano::vkh
{
	struct DescriptorPool;
	struct DescriptorSetLayout;

	struct DescriptorSets
	{
		DescriptorSets(DescriptorPool const& descriptorPool, DescriptorSetLayout const& layout, size_t size);
		DescriptorSets(DescriptorSets&&) noexcept;

		DescriptorSets& operator=(DescriptorSets&&) noexcept;

		vk::DescriptorSet handle(uint32_t index);
		vk::WriteDescriptorSet bind(uint32_t index, uint32_t binding, vk::DescriptorBufferInfo const& bufferInfo, uint32_t count = 1) const;
		vk::WriteDescriptorSet bind(uint32_t index, uint32_t binding, vk::DescriptorImageInfo const& imageInfo, uint32_t count = 1) const;
		vk::WriteDescriptorSet bind(uint32_t index, uint32_t binding, vk::WriteDescriptorSetAccelerationStructureNV const& structureInfo, uint32_t count = 1) const;

		void updateDescriptors(uint32_t index, std::vector<vk::WriteDescriptorSet> const& descriptorWrites) const;

		private:

			DescriptorPool const* descriptorPool;
			std::unordered_map<uint32_t, vk::DescriptorType> bindingTypes;
	public:
			std::vector<vk::DescriptorSet> descriptorSets;
	};
}