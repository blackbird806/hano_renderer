#pragma once

#include <set>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <core/hanoConfig.hpp>
#include "vkhUtility.hpp"
#include "vkhSurface.hpp"

namespace hano::vkh
{
	std::vector<vk::QueueFamilyProperties>::const_iterator findQueue(
		std::vector<vk::QueueFamilyProperties> const& queueFamilies,
		std::string const& name,
		VkQueueFlags requiredBits,
		VkQueueFlags excludedBits);
	
	struct Device
	{
		VULKAN_NON_COPIABLE_NON_MOVABLE(Device);

		Device(vk::PhysicalDevice physDevice, Surface const& isurface, std::vector<const char*> irequiredExtensions, vk::AllocationCallbacks* alloc);
		~Device();
		
		void checkRequiredExtensions(vk::PhysicalDevice physicalDevice) const;
		
		vk::Device handle;
		vk::PhysicalDevice const physicalDevice;
		vk::AllocationCallbacks const* allocator;
		Surface const& surface;
		std::vector<const char*> requiredExtensions;

		uint32 graphicsFamilyIndex() const { return m_graphicsFamilyIndex; }
		uint32 computeFamilyIndex() const { return m_computeFamilyIndex; }
		uint32 presentFamilyIndex() const { return m_presentFamilyIndex; }
		uint32 transferFamilyIndex() const { return m_transferFamilyIndex; }

		vk::Queue graphicsQueue() const { return m_graphicsQueue; }
		vk::Queue computeQueue() const { return m_computeQueue; }
		vk::Queue presentQueue() const { return m_presentQueue; }
		vk::Queue transferQueue() const { return m_transferQueue; }
		
	private:
		
		uint32 m_graphicsFamilyIndex;
		uint32 m_computeFamilyIndex;
		uint32 m_presentFamilyIndex;
		uint32 m_transferFamilyIndex;
		
		vk::Queue m_graphicsQueue;
		vk::Queue m_computeQueue;
		vk::Queue m_presentQueue;
		vk::Queue m_transferQueue;
	};
}
