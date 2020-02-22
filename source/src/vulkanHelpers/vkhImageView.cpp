#include <vulkanHelpers/vkhImageView.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

hano::vkh::ImageView::ImageView(Device const& idevice, vk::Image img, vk::Format fmt,
                                vk::ImageAspectFlagBits aspectFlags)
	: device(idevice), image(img)
{
	vk::ImageViewCreateInfo createInfo = {};
	createInfo.image = image;
	createInfo.viewType = vk::ImageViewType::e2D;
	createInfo.format = fmt;
	createInfo.components.r = vk::ComponentSwizzle::eIdentity;
	createInfo.components.g = vk::ComponentSwizzle::eIdentity;
	createInfo.components.b = vk::ComponentSwizzle::eIdentity;
	createInfo.components.a = vk::ComponentSwizzle::eIdentity;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	
	handle = device.handle.createImageView(createInfo, device.allocator);
}

hano::vkh::ImageView::~ImageView()
{
	if (handle)
	{
		device.handle.destroyImageView(handle, device.allocator);
		handle = nullptr;
	}
}
