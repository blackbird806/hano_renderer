#include <vulkanHelpers/vkhBuffer.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhDeviceMemory.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>

using namespace hano::vkh;

Buffer::Buffer(vkh::Device const& idevice, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProps)
	: device(&idevice), m_size(size)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	handle = device->handle.createBufferUnique(bufferInfo, device->allocator);

	auto const requirements = device->handle.getBufferMemoryRequirements(handle.get());

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize = requirements.size;
	allocInfo.memoryTypeIndex = vkh::DeviceMemory::findMemoryType(*device, requirements.memoryTypeBits, memoryProps);
	
	memory = device->handle.allocateMemoryUnique(allocInfo, device->allocator);
	device->handle.bindBufferMemory(handle.get(), memory.get(), 0);
}

void Buffer::copyFrom(CommandPool& commandPool, Buffer const& buffer, vk::DeviceSize size)
{
	SingleTimeCommands singleTimeCommands(commandPool);
	vk::BufferCopy copyRegion;
	copyRegion.size = size;
	singleTimeCommands.buffer().copyBuffer(buffer.handle.get(), handle.get(), 1, &copyRegion);
}

void* Buffer::map(vk::DeviceSize offset)
{
	return device->handle.mapMemory(memory.get(), offset, m_size, vk::MemoryMapFlagBits());
}

void Buffer::unMap()
{
	device->handle.unmapMemory(memory.get());
}