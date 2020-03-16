#pragma once

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "core/hanoConfig.hpp"
#include "vkhUtility.hpp"
#include "vkhDebug.hpp"

namespace hano::vkh
{
	std::vector<const char*> getGlfwRequiredExtensions(bool validationLayersEnabled);
	
	struct Instance
	{
		Instance(const char* appName, const char* engineName, std::vector<const char*> ivalidationLayers, vk::AllocationCallbacks* alloc, GLFWwindow* win);

		HANO_NODISCARD bool validationLayersEnabled() const noexcept;

		vk::UniqueInstance handle;
		vk::AllocationCallbacks* allocator;
		std::vector<vk::PhysicalDevice> physicalDevices;
		std::vector<vk::ExtensionProperties> extensions;
		std::vector<const char*> validationLayers;

		// @Review
		GLFWwindow* window;
		
		private:
		
		bool checkValidationLayerSupport();

	};
}
