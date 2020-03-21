#include <vulkanHelpers/vkhCommandPool.hpp>

using namespace hano::vkh;

CommandPool::CommandPool(vkh::Device const& device_, uint32 queueFamilyIndex, bool allowReset)
	: device(&device_)
{
	vk::CommandPoolCreateInfo poolInfo = {};
	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = allowReset ? vk::CommandPoolCreateFlagBits::eResetCommandBuffer : vk::CommandPoolCreateFlags();
	handle = device->handle.createCommandPoolUnique(poolInfo, device->allocator);
}

