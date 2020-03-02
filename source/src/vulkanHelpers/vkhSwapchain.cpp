#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <glfw/glfw3.h>
#include <vulkanHelpers/vkhSwapchain.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhImageView.hpp>

using namespace hano::vkh;

Swapchain::Swapchain(Device const& idevice, bool vsync, Swapchain* oldSwapchain)
	: device(idevice)
{
	auto const details = querySwapchainSupport(device.physicalDevice, device.surface.handle);
	if (details.formats.empty() || details.presentModes.empty())
	{
		throw HanoException("empty swap chain support");
	}

	const auto& surface = device.surface;
	const auto& window = surface.instance.window;

	const auto surfaceFormat = chooseSwapSurfaceFormat(details.formats);
	const auto presentMode = chooseSwapPresentMode(details.presentModes, vsync);
	extent = chooseSwapExtent(window, details.capabilities);
	const auto imageCount = chooseImageCount(details.capabilities);

	vk::SwapchainCreateInfoKHR createInfo = {};
	createInfo.surface = surface.handle;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = oldSwapchain ? oldSwapchain->handle : nullptr;

	if (device.graphicsFamilyIndex() != device.presentFamilyIndex())
	{
		uint32_t queueFamilyIndices[] = { device.graphicsFamilyIndex(), device.presentFamilyIndex() };

		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	VKH_CHECK(
		device.handle.createSwapchainKHR(&createInfo, nullptr, &handle),
	"failed to create swapchain !");

	minImageCount = details.capabilities.minImageCount;
	format = surfaceFormat.format;
	images = device.handle.getSwapchainImagesKHR(handle);
	imageViews.reserve(images.size());

	for (const auto image : images)
		imageViews.emplace_back(device, image, format, vk::ImageAspectFlagBits::eColor);
}

Swapchain::~Swapchain()
{
	if (handle)
	{
		// images must be destroyed before the swapchain
		imageViews.clear();
		device.handle.destroySwapchainKHR(handle, device.allocator);
	}
}

Swapchain::SupportDetails Swapchain::querySwapchainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
	SupportDetails details{
		.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface),
		.formats = physicalDevice.getSurfaceFormatsKHR(surface),
		.presentModes = physicalDevice.getSurfacePresentModesKHR(surface)
	};
	return details;
}

vk::SurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
{
	if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
	{
		return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto& format : formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}

	throw HanoException("found no suitable surface format");
}

vk::PresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes, bool vsync)
{
	// VK_PRESENT_MODE_IMMEDIATE_KHR: 
	//   Images submitted by your application are transferred to the screen right away, which may result in tearing.
	// VK_PRESENT_MODE_FIFO_KHR: 
	//   The swap chain is a queue where the display takes an image from the front of the queue when the display is
	//   refreshed and the program inserts rendered images at the back of the queue. If the queue is full then the program 
	//   has to wait. This is most similar to vertical sync as found in modern games. The moment that the display is 
	//   refreshed is known as "vertical blank".
	// VK_PRESENT_MODE_FIFO_RELAXED_KHR:
	//   This mode only differs from the previous one if the application is late and the queue was empty at the last 
	//   vertical blank. Instead of waiting for the next vertical blank, the image is transferred right away when it 
	//   finally arrives. This may result in visible tearing.
	// VK_PRESENT_MODE_MAILBOX_KHR: 
	//   This is another variation of the second mode. Instead of blocking the application when the queue is full, the 
	//   images that are already queued are simply replaced with the newer ones.This mode can be used to implement triple 
	//   buffering, which allows you to avoid tearing with significantly less latency issues than standard vertical sync 
	//   that uses double buffering.

	if (vsync)
	{
		return vk::PresentModeKHR::eFifo;
	}

	if (std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eMailbox) != presentModes.end())
	{
		return vk::PresentModeKHR::eMailbox;
	}

	if (std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eImmediate) != presentModes.end())
	{
		return vk::PresentModeKHR::eImmediate;
	}

	if (std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eFifoRelaxed) != presentModes.end())
	{
		return vk::PresentModeKHR::eFifoRelaxed;
	}

	return vk::PresentModeKHR::eFifo;
}


vk::Extent2D Swapchain::chooseSwapExtent(GLFWwindow* window, const vk::SurfaceCapabilitiesKHR& capabilities)
{
	// Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent member.
	// However, some window managers do allow us to differ here and this is indicated by setting the width and height in
	// currentExtent to a special value: the maximum value of uint32_t. In that case we'll pick the resolution that best 
	// matches the window within the minImageExtent and maxImageExtent bounds.
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	vk::Extent2D actualExtent = {static_cast<uint32>(w), static_cast<uint32>(h)};

	actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

	return actualExtent;
}

hano::uint32 Swapchain::chooseImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	// The implementation specifies the minimum amount of images to function properly
	// and we'll try to have one more than that to properly implement triple buffering.
	// (tanguyf: or not, we can just rely on VK_PRESENT_MODE_MAILBOX_KHR with two buffers)
	uint32 imageCount = capabilities.minImageCount;// +1; 

	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}

	return imageCount;
}