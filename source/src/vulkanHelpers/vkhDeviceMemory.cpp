#include <vulkanHelpers/vkhDeviceMemory.hpp>

using namespace hano::vkh;

DeviceMemory::DeviceMemory(DeviceMemory&& other) noexcept
	: device(other.device), handle(other.handle)
{
	other.handle = nullptr;
}

DeviceMemory::DeviceMemory(vkh::Device const& idevice, size_t size, uint32 memoryType, vk::MemoryPropertyFlags properties)
	: device(idevice)
{
	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize = size;
	allocInfo.memoryTypeIndex = findMemoryType(memoryType, properties);

	VKH_CHECK(
		device.handle.allocateMemory(&allocInfo, device.allocator, &handle), 
		"failed to allocate device memory !");
}

DeviceMemory::~DeviceMemory()
{
	if (handle)
	{
		device.handle.freeMemory(handle, device.allocator);
		handle = nullptr;
	}
}

void* DeviceMemory::map(size_t offset, size_t size)
{
	return device.handle.mapMemory(handle, offset, size);
}

void DeviceMemory::unmap()
{
	device.handle.unmapMemory(handle);
}

hano::uint32 DeviceMemory::findMemoryType(uint32 typeFilter, vk::MemoryPropertyFlags props) const
{
	vk::PhysicalDeviceMemoryProperties memProperties;
	device.physicalDevice.getMemoryProperties(&memProperties);

	for (uint32 i = 0; i != memProperties.memoryTypeCount; ++i)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & props) == props)
		{
			return i;
		}
	}
	
	throw HanoException("failed to find suitable memory type");
}
