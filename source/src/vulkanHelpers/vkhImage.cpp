#include <vulkanHelpers/vkhImage.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>

using namespace hano::vkh;

Image::Image(vkh::Device const& idevice, vk::Extent2D ext, vk::Format fmt, vk::ImageTiling tiling,
	vk::ImageUsageFlags usageFlags)
	: device(idevice), extent(ext), format(fmt), imageLayout(vk::ImageLayout::eUndefined)
{
	vk::ImageCreateInfo imageInfo = {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = extent.width;
	imageInfo.extent.height = extent.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = imageLayout;
	imageInfo.usage = usageFlags;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.samples = vk::SampleCountFlagBits::e1;

	handle = device.handle.createImage(imageInfo, device.allocator);
}

Image::~Image()
{
	if (handle)
	{
		device.handle.destroy(device.allocator);
		handle = nullptr;
	}
}

DeviceMemory Image::allocateMemory(vk::MemoryPropertyFlags propertyFlags) const
{
	auto const requirements = device.handle.getImageMemoryRequirements(handle);
	DeviceMemory memory(device, requirements.size, requirements.memoryTypeBits, propertyFlags);
	device.handle.bindImageMemory(handle, memory.handle, 0);
	return memory;
}

#if 0
void Image::transitionImageLayout(CommandPool& commandPool, vk::ImageLayout newLayout)
{
	SingleTimeCommands singleTimeCommands(commandPool);
	vk::ImageMemoryBarrier barrier = {};
	barrier.oldLayout = imageLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = handle;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

		if (DepthBuffer::HasStencilComponent(format))
		{
			barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (imageLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (imageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (imageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		throw HanoException("unsupported layout transition");
	}
	
	singleTimeCommands.buffer().pipelineBarrier(sourceStage, destinationStage, );
	vkCmdPipelineBarrier(, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
});

layout = newLayout;
}
#endif