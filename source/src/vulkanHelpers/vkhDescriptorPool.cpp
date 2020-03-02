#include <vulkanHelpers/vkhDescriptorPool.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

DescriptorPool::DescriptorPool(Device const& idevice, std::vector<DescriptorBinding> const& descriptorBindings, size_t maxSets)
	: device(&idevice)
{
	std::vector<vk::DescriptorPoolSize> poolSizes;

	for (const auto& binding : descriptorBindings)
		poolSizes.emplace_back(binding.type, static_cast<uint32_t>(binding.descriptorCount * maxSets));

	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(maxSets);

	handle = device->handle.createDescriptorPoolUnique(poolInfo, device->allocator);
}
