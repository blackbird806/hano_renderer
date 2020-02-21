#include <vulkanHelpers/vkhBuffer.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhDeviceMemory.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>

using namespace hano::vkh;

Buffer::Buffer(vkh::Device const& idevice, size_t size, vk::BufferUsageFlags usage)
	: device(idevice)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	VKH_CHECK(
		device.handle.createBuffer(&bufferInfo, device.allocator, &handle),
		"failed to create buffer !");
}

Buffer::~Buffer()
{
	if (handle)
	{
		device.handle.destroyBuffer(handle, device.allocator);
		handle = nullptr;
	}
}

DeviceMemory Buffer::allocateMemory(vk::MemoryPropertyFlags props)
{
	auto const requirements = device.handle.getBufferMemoryRequirements(handle);
	DeviceMemory memory(device, requirements.size, requirements.memoryTypeBits, props);
	device.handle.bindBufferMemory(handle, memory.handle, 0);
	return memory;
}

void Buffer::copyFrom(CommandPool& commandPool, Buffer const& buffer, vk::DeviceSize size)
{
	SingleTimeCommands singleTimeCommands(commandPool);
	vk::BufferCopy copyRegion;
	copyRegion.size = size;

	singleTimeCommands.buffer().copyBuffer(buffer.handle, handle, 1, &copyRegion);
}
