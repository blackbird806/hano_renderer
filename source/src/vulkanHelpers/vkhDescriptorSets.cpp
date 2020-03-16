#include <vulkanHelpers/vkhDescriptorSets.hpp>
#include <vulkanHelpers/vkhDescriptorPool.hpp>
#include <vulkanHelpers/vkhDescriptorSetLayout.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <core/hanoException.hpp>

using namespace hano::vkh;

DescriptorSets::DescriptorSets(DescriptorPool const& descriptorPool_, DescriptorSetLayout const& layout, size_t size)
	: descriptorPool(&descriptorPool_)
{
	// Sanity check to avoid binding different resources to the same binding point.
	for (auto const& binding : layout.getDescriptorBindings())
	{
		if (!bindingTypes.insert(std::make_pair(binding.binding, binding.type)).second)
		{
			throw HanoException("binding collision");
		}
	}

	std::vector<vk::DescriptorSetLayout> layouts(size, layout.handle.get());

	vk::DescriptorSetAllocateInfo allocInfo = {};
	allocInfo.descriptorPool = descriptorPool->handle.get();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(size);
	descriptorSets = descriptorPool->device->handle.allocateDescriptorSets(allocInfo);
}

DescriptorSets::DescriptorSets(DescriptorSets&& other) noexcept
	: descriptorPool(other.descriptorPool),
	bindingTypes(std::move(other.bindingTypes)),
	descriptorSets(std::move(other.descriptorSets))
{

}

DescriptorSets& DescriptorSets::operator=(DescriptorSets&& other) noexcept
{
	descriptorPool = other.descriptorPool;
	bindingTypes = std::move(other.bindingTypes);
	descriptorSets = std::move(other.descriptorSets);
	return *this;
}

vk::WriteDescriptorSet DescriptorSets::bind(uint32_t index, uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo, uint32_t count) const 
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindingTypes.at(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pBufferInfo = &bufferInfo;

	return descriptorWrite;
}

vk::WriteDescriptorSet DescriptorSets::bind(uint32_t index, uint32_t binding, const vk::DescriptorImageInfo& imageInfo, uint32_t count) const 
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindingTypes.at(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pImageInfo = &imageInfo;

	return descriptorWrite;
}

vk::WriteDescriptorSet DescriptorSets::bind(uint32_t index, uint32_t binding, const vk::WriteDescriptorSetAccelerationStructureNV& structureInfo, uint32_t count) const 
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindingTypes.at(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pNext = &structureInfo;

	return descriptorWrite;
}

void DescriptorSets::updateDescriptors(uint32_t index, std::vector<vk::WriteDescriptorSet> const& descriptorWrites) const
{
	descriptorPool->device->handle.updateDescriptorSets(descriptorWrites, {});
}

vk::DescriptorSet DescriptorSets::handle(uint32_t index)
{
	return descriptorSets[index];
}