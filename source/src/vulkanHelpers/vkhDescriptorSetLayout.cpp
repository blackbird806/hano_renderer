#include <vulkanHelpers/vkhDescriptorSetLayout.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

DescriptorSetLayout::DescriptorSetLayout()
	: device(nullptr), handle(nullptr)
{

}

DescriptorSetLayout::DescriptorSetLayout(Device const& device_, std::vector<DescriptorBinding> const& descriptorBindings_)
{
	init(device_, descriptorBindings_);
}

void DescriptorSetLayout::init(Device const& device_, std::vector<DescriptorBinding> const& descriptorBindings_)
{
	device = &device_;
	m_descriptorBindings = descriptorBindings_;
	std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;

	for (const auto& binding : m_descriptorBindings)
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

void DescriptorSetLayout::destroy()
{
	handle.reset();
}

std::vector<DescriptorBinding> const& DescriptorSetLayout::getDescriptorBindings() const noexcept
{
	return m_descriptorBindings;
}
