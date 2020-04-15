#pragma once

#include <unordered_map>
#include <vector>
#include <span>
#include <vulkan/vulkan.hpp>
#include <core/hanoConfig.hpp>
#include "vkhDescriptorBinding.hpp"

namespace hano::vkh
{
	struct DescriptorPool;
	struct DescriptorSetLayout;

	struct DescriptorSets
	{
		DescriptorSets();
		DescriptorSets(DescriptorPool const& descriptorPool, DescriptorSetLayout const& layout, size_t size);
		DescriptorSets(DescriptorSets&&) noexcept;

		void init(DescriptorPool const& descriptorPool, DescriptorSetLayout const& layout, size_t size);
		
		// WARN /!\ does not free the descriptor sets
		void destroy();

		DescriptorSets& operator=(DescriptorSets&&) noexcept;

		HANO_NODISCARD vk::DescriptorSet handle(uint32 index);
		void push(uint32 index, uint32 binding, vk::DescriptorBufferInfo const& bufferInfo, uint32 count = 1);
		void push(uint32 index, uint32 binding, vk::DescriptorImageInfo const& imageInfo, uint32 count = 1); // @Deprecated
		void push(uint32 index, uint32 binding, std::span<vk::DescriptorImageInfo> imageInfo);
		void push(uint32 index, uint32 binding, vk::WriteDescriptorSetAccelerationStructureNV const& structureInfo, uint32 count = 1);

		void updateDescriptors(uint32 index);
		void bindDescriptor(uint32 index, vk::CommandBuffer commandBuffer, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout);

		private:

			DescriptorPool const* descriptorPool;
			std::unordered_map<uint32, vk::DescriptorType> bindingTypes;
			std::vector<vk::DescriptorSet> descriptorSets;
			std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
	};
}