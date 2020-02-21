#include <vector>
#include <vulkanHelpers/vkhDepthBuffer.hpp>
#include <vulkanHelpers/vkhCommandPool.hpp>
#include <core/hanoException.hpp>

using namespace hano::vkh;

namespace
{
	vk::Format findSupportedFormat(const Device& device, const std::vector<vk::Format>& candidates, const vk::ImageTiling tiling, const vk::FormatFeatureFlags features)
	{
		for (auto format : candidates)
		{
			vk::FormatProperties props = device.physicalDevice.getFormatProperties(format);;
			
			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}

			if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		::hano::HanoException("failed to find supported format");
		return vk::Format();
	}

	vk::Format findDepthFormat(const Device& device)
	{
		return findSupportedFormat(
			device,
			{	vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
				vk::ImageTiling::eOptimal,
				vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}
}

DepthBuffer::DepthBuffer(CommandPool& commandPool, vk::Extent2D extent)
	:	image(commandPool.device, extent, findDepthFormat(commandPool.device), vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment),
		imageView(commandPool.device, image.handle, image.format, vk::ImageAspectFlagBits::eDepth),
		imageMemory(image.allocateMemory(vk::MemoryPropertyFlagBits::eDeviceLocal))
{
	image.transitionImageLayout(commandPool, vk::ImageLayout::eDepthAttachmentOptimal);
}

bool DepthBuffer::hasStencilComponent(vk::Format format) noexcept
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}
