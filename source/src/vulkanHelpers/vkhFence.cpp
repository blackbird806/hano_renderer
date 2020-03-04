#include <vulkanHelpers/vkhFence.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

Fence::Fence(vkh::Device const& idevice, bool signal)
    : device(&idevice)
{
    vk::FenceCreateInfo fenceInfo = {};
    fenceInfo.flags = signal ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlagBits();
    
    handle = device->handle.createFenceUnique(fenceInfo, device->allocator);
}

void Fence::reset()
{
    device->handle.resetFences(handle.get());
}

void Fence::wait(uint64 timeout)
{
    device->handle.waitForFences(handle.get(), VK_TRUE, timeout);
}
