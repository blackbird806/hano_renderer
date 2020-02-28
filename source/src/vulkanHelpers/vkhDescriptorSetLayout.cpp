#include <vulkanHelpers/vkhDescriptorSetLayout.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

DescriptorSetLayout::DescriptorSetLayout(Device const& idevice, std::vector<DescriptorBinding> const& descriptorBindings)
	: device(&idevice)
{
	std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;

	for (const auto& binding : descriptorBindings)
	{
		vk::DescriptorSetLayoutBinding b = {};
		b.binding = binding.binding;
		b.descriptorCount = binding.descriptorCount;
		b.descriptorType = binding.type;
		b.stageFlags = binding.stage;
		layoutBindings.push_back(b);
	}

	vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	layoutInfo.pBindings = layoutBindings.data();

	handle = device->handle.createDescriptorSetLayoutUnique(layoutInfo, device->allocator);
}