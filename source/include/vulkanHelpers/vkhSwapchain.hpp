#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <vulkan/vulkan.hpp>
#include "vkhUtility.hpp"
#include <core/hanoConfig.hpp>

struct GLFWwindow;

namespace hano::vkh
{
	struct Device;
	struct ImageView;
	
	struct Swapchain
	{
		VULKAN_NON_COPIABLE_NON_MOVABLE(Swapchain);

		Swapchain(Device const& idevice, bool vsync);
		~Swapchain();
		
		struct SupportDetails
		{
			vk::SurfaceCapabilitiesKHR capabilities{};
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		};

		static SupportDetails querySwapchainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
		static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
		static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes, bool vsync);
		static vk::Extent2D chooseSwapExtent(GLFWwindow* window, const vk::SurfaceCapabilitiesKHR& capabilities);
		static uint32 chooseImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);
		
		Device const& device;
		uint32 minImageCount;
		vk::SwapchainKHR handle;
		vk::Format format;
		vk::Extent2D extent;
		std::vector<vk::Image> images;
		std::vector<ImageView> imageViews;
	};
}
