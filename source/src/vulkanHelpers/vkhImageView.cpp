#include <vulkanHelpers/vkhImageView.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

ImageView::ImageView() : handle(nullptr), device(nullptr), image(nullptr)
{

}

ImageView::ImageView(Device const& device_, vk::Image img, vk::Format fmt,
                                vk::ImageAspectFlagBits aspectFlags)
{
	init(device_, img, fmt, aspectFlags);
}

void ImageView::init(Device const& device_, vk::Image img, vk::Format fmt, vk::ImageAspectFlagBits aspectFlags)
{
	device = &device_;
	image = img;

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

	handle = device->handle.createImageViewUnique(createInfo, device->allocator);
}

void ImageView::destroy()
{
	handle.reset();
	device = nullptr;
}
