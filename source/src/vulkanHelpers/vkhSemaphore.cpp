#include <vulkanHelpers/vkhSemaphore.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

Semaphore::Semaphore(Device const& idevice)
	: device(&idevice)
{
	vk::SemaphoreCreateInfo semaphoreInfo = {};
	handle = device->handle.createSemaphoreUnique(semaphoreInfo, device->allocator);
}
