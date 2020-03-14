#pragma once

#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace hano::vkh
{
	struct DescriptorPool;
	struct DescriptorSetLayout;

	struct DescriptorSets
	{
		DescriptorSets(DescriptorPool const& descriptorPool, DescriptorSetLayout const& layout,
			std::unordered_map<uint32_t, vk::DescriptorType> const& bindingTypes, size_t size);

		vk::DescriptorSet handle(uint32_t index);
		vk::WriteDescriptorSet bind(uint32_t index, uint32_t binding, vk::DescriptorBufferInfo const& bufferInfo, uint32_t count = 1) const;
		vk::WriteDescriptorSet bind(uint32_t index, uint32_t binding, vk::DescriptorImageInfo const& imageInfo, uint32_t count = 1) const;
		vk::WriteDescriptorSet bind(uint32_t index, uint32_t binding, vk::WriteDescriptorSetAccelerationStructureNV const& structureInfo, uint32_t count = 1) const;

		void updateDescriptors(uint32_t index, std::vector<vk::WriteDescriptorSet> const& descriptorWrites);

		private:

			DescriptorPool const* descriptorPool;
			std::unordered_map<uint32_t, vk::DescriptorType> bindigTypes;
			std::vector<vk::DescriptorSet> descriptorSets;
	};
}