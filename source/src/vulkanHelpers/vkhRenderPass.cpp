#include <vulkan/vulkan.hpp>
#include <vulkanHelpers/vkhRenderPass.hpp>
#include <vulkanHelpers/vkhSwapchain.hpp>
#include <vulkanHelpers/vkhDepthBuffer.hpp>

using namespace hano::vkh;

RenderPass::RenderPass(Swapchain const& iswapchain, DepthBuffer const& idepthBuffer, bool clearColorBuffer, bool clearDepthBuffer)
	: swapchain(&iswapchain), depthBuffer(&idepthBuffer)
{
	vk::AttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchain->format;
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = clearColorBuffer ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentDescription depthAttachment = {};
	depthAttachment.format = depthBuffer->image.format;
	depthAttachment.samples = vk::SampleCountFlagBits::e1;
	depthAttachment.loadOp = clearDepthBuffer ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
	depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
	depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::AttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::AttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::SubpassDescription subpass = {};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	vk::SubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.srcAccessMask = vk::AccessFlagBits();
	dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	std::array<vk::AttachmentDescription, 2> attachments =
	{
		colorAttachment,
		depthAttachment
	};

	vk::RenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.attachmentCount = static_cast<uint32>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	handle = swapchain->device.handle.createRenderPassUnique(renderPassInfo, swapchain->device.allocator);
}

