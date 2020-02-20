#pragma once

#include <set>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <core/hanoConfig.hpp>
#include "vkhUtility.hpp"
#include "vkhSurface.hpp"

namespace hano::vkh
{
	inline std::vector<vk::QueueFamilyProperties>::const_iterator findQueue(
		const std::vector<vk::QueueFamilyProperties>& queueFamilies,
		const std::string& name,
		const VkQueueFlags requiredBits,
		const VkQueueFlags excludedBits)
	{
		const auto family = std::find_if(queueFamilies.begin(), queueFamilies.end(), [requiredBits, excludedBits](const VkQueueFamilyProperties& queueFamily)
			{
				return
					queueFamily.queueCount > 0 &&
					queueFamily.queueFlags & requiredBits &&
					!(queueFamily.queueFlags & excludedBits);
			});

		if (family == queueFamilies.end())
			throw HanoException("found no matching " + name + " queue");

		return family;
	}
	
	struct Device
	{
		VULKAN_NON_COPIABLE(Device);

		Device(vk::PhysicalDevice physDevice, Surface const& isurface, std::vector<const char*> irequiredExtensions)
			: physicalDevice(physDevice), surface(isurface), requiredExtensions(irequiredExtensions)
		{
			checkRequiredExtensions(physicalDevice);

			auto const queueFamilies = physicalDevice.getQueueFamilyProperties();
			
			// Find the graphics queue.
			auto const graphicsFamily = findQueue(queueFamilies, "graphics", VK_QUEUE_GRAPHICS_BIT, 0);
			auto const computeFamily = findQueue(queueFamilies, "compute", VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
			auto const transferFamily = findQueue(queueFamilies, "transfer", VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

			// Find the presentation queue (usually the same as graphics queue).
			auto const presentFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [&](const vk::QueueFamilyProperties& queueFamily)
				{
					VkBool32 presentSupport = false;
					uint32 const i = static_cast<uint32>(&*queueFamilies.cbegin() - &queueFamily);
					vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface.handle, &presentSupport);
					return queueFamily.queueCount > 0 && presentSupport;
				});

			if (presentFamily == queueFamilies.end())
				throw HanoException("found no presentation queue");

			graphicsFamilyIndex = static_cast<uint32>(graphicsFamily - queueFamilies.begin());
			computeFamilyIndex = static_cast<uint32>(computeFamily - queueFamilies.begin());
			presentFamilyIndex = static_cast<uint32>(presentFamily - queueFamilies.begin());
			transferFamilyIndex = static_cast<uint32>(transferFamily - queueFamilies.begin());

			// Queues can be the same
			const std::set<uint32> uniqueQueueFamilies =
			{
				graphicsFamilyIndex,
				computeFamilyIndex,
				presentFamilyIndex,
				transferFamilyIndex
			};

			// Create queues
			float queuePriority = 1.0f;
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

			for (uint32 queueFamilyIndex : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;

				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkPhysicalDeviceFeatures deviceFeatures = {};
			deviceFeatures.fillModeNonSolid = true;
			deviceFeatures.samplerAnisotropy = true;

			VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures = {};
			indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
			indexingFeatures.runtimeDescriptorArray = true;

			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pNext = &indexingFeatures;
			createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledLayerCount = static_cast<uint32>(surface.instance.validationLayers.size());
			createInfo.ppEnabledLayerNames = surface.instance.validationLayers.data();
			createInfo.enabledExtensionCount = static_cast<uint32>(requiredExtensions.size());
			createInfo.ppEnabledExtensionNames = requiredExtensions.data();

			handle = physicalDevice.createDevice(createInfo, allocator);

			handle.getQueue(graphicsFamilyIndex, 0, &graphicsQueue);
			handle.getQueue(computeFamilyIndex, 0, &computeQueue);
			handle.getQueue(presentFamilyIndex, 0, &presentQueue);
			handle.getQueue(transferFamilyIndex, 0, &transferQueue);
		}

		~Device()
		{
			if (handle != VK_NULL_HANDLE)
			{
				handle.destroy(allocator);
				handle = nullptr;
			}
		}
		
		void checkRequiredExtensions(vk::PhysicalDevice physicalDevice) const
		{
			auto const availableExtensions = physicalDevice.enumerateDeviceExtensionProperties(nullptr);
			std::set<std::string> requiredExtensions(requiredExtensions.begin(), requiredExtensions.end());

			for (const auto& extension : availableExtensions)
				requiredExtensions.erase(extension.extensionName);

			if (!requiredExtensions.empty())
			{
				bool first = true;
				std::string extensions;

				for (const auto& extension : requiredExtensions)
				{
					if (!first)
						extensions += ", ";

					extensions += extension;
					first = false;
				}

				throw HanoException("missing required extensions: " + extensions);
			}
		}
		
		vk::Device handle;
		vk::PhysicalDevice const physicalDevice;
		vk::AllocationCallbacks* allocator;

		Surface const& surface;
		
		uint32 graphicsFamilyIndex;
		uint32 computeFamilyIndex;
		uint32 presentFamilyIndex;
		uint32 transferFamilyIndex;
		
		vk::Queue graphicsQueue;
		vk::Queue computeQueue;
		vk::Queue transferQueue;
		vk::Queue presentQueue;

		std::vector<const char*> requiredExtensions;
	};
}
