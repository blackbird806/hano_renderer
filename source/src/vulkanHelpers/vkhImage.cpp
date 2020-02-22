#include <vulkanHelpers/vkhImage.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>
#include <vulkanHelpers/vkhDepthBuffer.hpp>
#include <vulkanHelpers/vkhBuffer.hpp>

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

		if (DepthBuffer::hasStencilComponent(format))
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

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (imageLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		
		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (imageLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else
	{
		throw HanoException("unsupported layout transition");
	}
	
	singleTimeCommands.buffer().pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

	imageLayout = newLayout;
}

void Image::copyFrom(CommandPool& commandPool, vkh::Buffer const& buffer)
{
	SingleTimeCommands singleTimeCommands(commandPool);

	vk::BufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = vk::Offset3D{ 0, 0, 0 };
	region.imageExtent = vk::Extent3D{ extent.width, extent.height, 1 };

	singleTimeCommands.buffer().copyBufferToImage(buffer.handle, handle, vk::ImageLayout::eTransferDstOptimal, { region });
}
