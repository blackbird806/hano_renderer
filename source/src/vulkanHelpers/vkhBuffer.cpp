#include <vulkanHelpers/vkhBuffer.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhDeviceMemory.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>

using namespace hano::vkh;

Buffer::Buffer()
	: handle(nullptr), memory(nullptr)
{

}

Buffer::Buffer(vkh::Device const& device_, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProps)
{
	init(device_, size, usage, memoryProps);
}

void Buffer::init(vkh::Device const& device_, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProps)
{
	assert(!handle);
	assert(!memory);

	m_size = size;
	device = &device_;

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

void Buffer::destroy()
{
	handle.reset();
	memory.reset();
}

Buffer::operator bool() const noexcept
{
	return static_cast<bool>(handle);
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

vk::DeviceSize Buffer::getSize() const noexcept
{
	return m_size;
}