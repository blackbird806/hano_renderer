#include <vulkanHelpers/vkhDescriptorSets.hpp>
#include <vulkanHelpers/vkhDescriptorPool.hpp>
#include <vulkanHelpers/vkhDescriptorSetLayout.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <core/hanoException.hpp>

using namespace hano::vkh;

DescriptorSets::DescriptorSets()
	: descriptorPool(nullptr)
{

}

DescriptorSets::DescriptorSets(DescriptorPool const& descriptorPool_, DescriptorSetLayout const& layout, size_t size)
{
	init(descriptorPool_, layout, size);
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

void DescriptorSets::init(DescriptorPool const& descriptorPool_, DescriptorSetLayout const& layout, size_t size)
{
	assert(descriptorSets.empty());

	descriptorPool = &descriptorPool_;
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

void DescriptorSets::destroy()
{
	descriptorPool = nullptr;
	descriptorSets.clear();
	writeDescriptorSets.clear();
	bindingTypes.clear();
}

void DescriptorSets::push(uint32 index, uint32 binding, const vk::DescriptorBufferInfo& bufferInfo, uint32 count)
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindingTypes.at(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pBufferInfo = &bufferInfo;
	
	writeDescriptorSets.push_back(descriptorWrite);
}

void DescriptorSets::push(uint32 index, uint32 binding, const vk::DescriptorImageInfo& imageInfo, uint32 count)
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindingTypes.at(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pImageInfo = &imageInfo;

	writeDescriptorSets.push_back(descriptorWrite);
}

void DescriptorSets::push(uint32 index, uint32 binding, const vk::WriteDescriptorSetAccelerationStructureNV& structureInfo, uint32 count)
{
	vk::WriteDescriptorSet descriptorWrite = {};
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = bindingTypes.at(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pNext = &structureInfo;

	writeDescriptorSets.push_back(descriptorWrite);
}

void DescriptorSets::updateDescriptors(uint32 index)
{
	descriptorPool->device->handle.updateDescriptorSets(writeDescriptorSets, {});
	writeDescriptorSets.clear();
}

void DescriptorSets::bindDescriptor(uint32 index, vk::CommandBuffer commandBuffer, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout)
{
	commandBuffer.bindDescriptorSets(bindPoint, pipelineLayout, 0, descriptorSets[index], {});
}

vk::DescriptorSet DescriptorSets::handle(uint32 index)
{
	return descriptorSets[index];
}