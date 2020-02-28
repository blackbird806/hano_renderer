#include <vulkanHelpers/vkhFrameBuffer.hpp>
#include <vulkanHelpers/vkhImageView.hpp>
#include <vulkanHelpers/vkhRenderPass.hpp>
#include <vulkanHelpers/vkhDepthBuffer.hpp>
#include <vulkanHelpers/vkhSwapchain.hpp>

using namespace hano::vkh;

FrameBuffer::FrameBuffer(vkh::ImageView const& iimageView, vkh::RenderPass const& irenderPass)
	: imageView(&iimageView), renderPass(&irenderPass)
{
	std::array<vk::ImageView, 2> attachments =
	{
		imageView->handle,
		renderPass->depthBuffer->imageView.handle
	};

	vk::FramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.renderPass = renderPass->handle.get();
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = renderPass->swapchain->extent.width;
	framebufferInfo.height = renderPass->swapchain->extent.height;
	framebufferInfo.layers = 1;

	handle = imageView->device.handle.createFramebufferUnique(framebufferInfo, imageView->device.allocator);
}
