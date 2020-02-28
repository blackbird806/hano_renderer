#include <vulkanHelpers/vkhBuffer.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhDeviceMemory.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>

using namespace hano::vkh;

Buffer::Buffer(vkh::Device const& idevice, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProps)
	: device(idevice)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	VKH_CHECK(
		device.handle.createBuffer(&bufferInfo, device.allocator, &handle),
		"failed to create buffer !");

	auto const requirements = device.handle.getBufferMemoryRequirements(handle);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize = requirements.size;
	allocInfo.memoryTypeIndex = vkh::DeviceMemory::findMemoryType(device, requirements.memoryTypeBits, memoryProps);
	VKH_CHECK(
		device.handle.allocateMemory(&allocInfo, device.allocator, &memory),
		"failed to allocate device memory !");
	device.handle.bindBufferMemory(handle, memory, 0);
}

Buffer::~Buffer()
{
	if (handle)
	{
		assert(memory);
		device.handle.destroyBuffer(handle, device.allocator);
		device.handle.freeMemory(memory, device.allocator);
		handle = nullptr;
		memory = nullptr;
	}
}

void Buffer::copyFrom(CommandPool& commandPool, Buffer const& buffer, vk::DeviceSize size)
{
	SingleTimeCommands singleTimeCommands(commandPool);
	vk::BufferCopy copyRegion;
	copyRegion.size = size;

	singleTimeCommands.buffer().copyBuffer(buffer.handle, handle, 1, &copyRegion);
}
