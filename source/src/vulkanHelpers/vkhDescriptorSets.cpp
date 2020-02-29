#include <vulkanHelpers/vkhDescriptorSets.hpp>
#include <vulkanHelpers/vkhDescriptorPool.hpp>
#include <vulkanHelpers/vkhDescriptorSetLayout.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <core/hanoException.hpp>

using namespace hano::vkh;

DescriptorSets::DescriptorSets(DescriptorPool const& idescriptorPool, DescriptorSetLayout const& layout,
	std::unordered_map<uint32_t, vk::DescriptorType> const& bindingTypes, size_t size)
	: descriptorPool(&idescriptorPool)
{
	std::vector<vk::DescriptorSetLayout> layouts(size, layout.handle.get());

	vk::DescriptorSetAllocateInfo allocInfo = {};
	allocInfo.descriptorPool = descriptorPool->handle.get();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(size);
	descriptorSets = descriptorPool->device->handle.allocateDescriptorSetsUnique(allocInfo);
}

vk::WriteDescriptorSet DescriptorSets::bind(uint32_t index, uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo, uint32_t count) const 
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index].get();
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindigTypes.at(index);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pBufferInfo = &bufferInfo;

	return descriptorWrite;
}

vk::WriteDescriptorSet DescriptorSets::bind(uint32_t index, uint32_t binding, const vk::DescriptorImageInfo& imageInfo, uint32_t count) const 
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index].get();
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindigTypes.at(index);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pImageInfo = &imageInfo;

	return descriptorWrite;
}

vk::WriteDescriptorSet DescriptorSets::bind(uint32_t index, uint32_t binding, const vk::WriteDescriptorSetAccelerationStructureNV& structureInfo, uint32_t count) const 
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index].get();
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindigTypes.at(index);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pNext = &structureInfo;

	return descriptorWrite;
}

void DescriptorSets::UpdateDescriptors(uint32_t index, std::vector<vk::WriteDescriptorSet> const& descriptorWrites)
{
	descriptorPool->device->handle.updateDescriptorSets(descriptorWrites, {});
}
