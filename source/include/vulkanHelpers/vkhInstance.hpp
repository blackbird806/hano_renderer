#pragma once

#include <string_view>
#include <array>
#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "core/hanoConfig.hpp"
#include "vkhUtility.hpp"
#include "vkhDebug.hpp"

namespace hano::vkh
{
	std::vector<const char*> getGlfwRequiredExtensions(bool validationLayersEnabled)
	{
		uint32 glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		// @TODO handle this in an elegant way
		// Needed to enable VK_NV_RAY_TRACING_EXTENSION_NAME
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		if (validationLayersEnabled)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}
	
	struct Instance
	{
		VULKAN_NON_COPIABLE(Instance);

		Instance(const char* appName, const char* engineName, std::vector<std::string_view> ivalidationLayers, vk::AllocationCallbacks alloc)
			: allocator(alloc), validationLayers(ivalidationLayers)
		{
			// disable validation layers if none provided
			if (validationLayersEnabled() && !checkValidationLayerSupport())
				throw hano::HanoException("validation layers requested, but not available!");

			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = appName;
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = engineName;
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			auto extensions = getGlfwRequiredExtensions(validationLayersEnabled());
			
			createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};

			if (validationLayersEnabled())
			{
				createInfo.enabledLayerCount = static_cast<uint32>(c_vulkanValidationLayers.size());
				createInfo.ppEnabledLayerNames = c_vulkanValidationLayers.data();
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				debugCreateInfo.pfnUserCallback = debugVkCallback;
				debugCreateInfo.pUserData = nullptr;
				createInfo.pNext = &debugCreateInfo;
			}
			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.pNext = nullptr;
			}

			instance = vk::createInstance(createInfo, &allocator);
		}

		HANO_NODISCARD bool validationLayersEnabled() const noexcept
		{
			return !validationLayers.empty();
		}

		~Instance()
		{
			if (instance != VK_NULL_HANDLE)
			{
				instance.destroy(allocator);
				instance = nullptr;
			}
		}
		
		vk::Instance instance;
		vk::AllocationCallbacks allocator;
		
		private:

		bool checkValidationLayerSupport()
		{
			uint32 layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::vector<VkLayerProperties> availablesLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availablesLayers.data());

			for (auto const& layerName : validationLayers)
			{
				bool layerFound = false;
				for (auto const& layerProperties : availablesLayers)
				{
					if (layerName.compare(layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}
				if (!layerFound)
					return false;
			}
			return true;
		}

		std::vector<std::string_view> validationLayers;
	};
}
