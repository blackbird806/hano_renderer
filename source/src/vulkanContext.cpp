#include <vulkanContext.hpp>
#undef max

using namespace hano;

void VulkanContext::init(GLFWwindow* window, VulkanConfig const& config)
{
	assert(window);
	m_window = window;
	instance = std::make_unique<vkh::Instance>(config.appName, config.engineName,
		std::vector(c_vulkanValidationLayers.begin(), c_vulkanValidationLayers.end()),
		vkAllocator, window);

	surface = std::make_unique<vkh::Surface>(*instance, window, vkAllocator);
	device = std::make_unique<vkh::Device>(getSuitableDevice(), *surface, std::vector(c_vulkanDefaultRequiredExtentions.begin(), c_vulkanDefaultRequiredExtentions.end()), vkAllocator);
	commandPool = std::make_unique<vkh::CommandPool>(*device, device->graphicsFamilyIndex(), true);
	createSwapchain();
}

std::vector<vk::PhysicalDevice> const& VulkanContext::getPhysicalDevices() const
{
	return instance->physicalDevices;
}

std::vector<vk::ExtensionProperties> const& VulkanContext::getExtensions() const
{
	return instance->extensions;
}

vk::PhysicalDevice VulkanContext::getSuitableDevice() const
{
	auto const& physicalDevices = getPhysicalDevices();
	auto const result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](vk::PhysicalDevice const& device)
		{
			// We want a device with geometry shader support.
			vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

			if (!deviceFeatures.geometryShader)
			{
				return false;
			}

			// We want a device with a graphics queue.
			auto const queueFamilies = device.getQueueFamilyProperties();

			auto const hasGraphicsQueue = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const vk::QueueFamilyProperties& queueFamily)
				{
					return queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics;
				});

			return hasGraphicsQueue != queueFamilies.end();
		});

	if (result == physicalDevices.end())
		throw HanoException("cannot find a suitable device !");

	return *result;
}

void VulkanContext::createSwapchain()
{
	swapchain = std::make_unique<vkh::Swapchain>(*device, false /*TODO*/ , swapchain.get());
	depthBuffer = std::make_unique<vkh::DepthBuffer>(*commandPool, swapchain->extent);

	for (uint32 i = 0; i < swapchain->imageViews.size(); i++)
	{
		m_imageAvailableSemaphores.emplace_back(*device);
		m_renderFinishedSemaphores.emplace_back(*device);
		m_inFlightFences.emplace_back(*device, true);
	}

	graphicsPipeline = std::make_unique<vkh::GraphicsPipeline>(*swapchain, *depthBuffer, false);
	
	descriptorPool = std::make_unique<vkh::DescriptorPool>(*device, graphicsPipeline->descriptorSetLayout.getDescriptorBindings(), /*@TODO*/500);

	for (auto const& imageView : swapchain->imageViews)
	{
		swapchainFrameBuffers.emplace_back(imageView, graphicsPipeline->renderPass);
	}

	commandBuffers = std::make_unique<vkh::CommandBuffers>(*commandPool, swapchainFrameBuffers.size());
}

void VulkanContext::deleteSwapchain()
{
	commandBuffers.reset();
	swapchainFrameBuffers.clear();
	graphicsPipeline.reset();
	m_inFlightFences.clear();
	m_renderFinishedSemaphores.clear();
	m_imageAvailableSemaphores.clear();
	depthBuffer.reset();
	swapchain.reset();
}

void VulkanContext::recreateSwapchain()
{
	// @Review
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_window, &width, &height);
	while (width == 0 || height == 0) 
	{
		glfwGetFramebufferSize(m_window, &width, &height);
		glfwWaitEvents();
	}

	device->handle.waitIdle();
	deleteSwapchain();
	createSwapchain();
	onRecreateSwapchain();
}

std::optional<vk::CommandBuffer> VulkanContext::beginFrame()
{
	auto constexpr noTimeout = std::numeric_limits<uint64>::max();
	
	auto& inFlightFence = m_inFlightFences[m_currentFrame];
	auto imageAvailableSemaphore = m_imageAvailableSemaphores[m_currentFrame].handle.get();
	auto renderFinishedSemaphore = m_renderFinishedSemaphores[m_currentFrame].handle.get();

	inFlightFence.wait(noTimeout);

	auto result = device->handle.acquireNextImageKHR(swapchain->handle, noTimeout, imageAvailableSemaphore, nullptr);
	imageIndex = result.value;
	m_result = result.result;

	if (result.result == vk::Result::eErrorOutOfDateKHR || result.result == vk::Result::eSuboptimalKHR || frameBufferResized)
	{
		frameBufferResized = false;
		recreateSwapchain();

		// @TODO
		return {};
	} 
	else if (result.result != vk::Result::eSuccess)
	{
		throw HanoException(std::string("failed to acquire next image (") + to_string(result.result) + ")");
	}

	commandBuffer = commandBuffers->begin(imageIndex);

	return { commandBuffer };
}

void VulkanContext::endFrame()
{
	auto& inFlightFence = m_inFlightFences[m_currentFrame];
	auto imageAvailableSemaphore = m_imageAvailableSemaphores[m_currentFrame].handle.get();
	auto renderFinishedSemaphore = m_renderFinishedSemaphores[m_currentFrame].handle.get();

	commandBuffers->end(imageIndex);

	vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore };
	vk::CommandBuffer cmdBuff[] = { commandBuffer };

	vk::SubmitInfo submitInfo = {};
	submitInfo.waitSemaphoreCount = std::size(waitSemaphores);
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = std::size(cmdBuff);
	submitInfo.pCommandBuffers = cmdBuff;
	submitInfo.signalSemaphoreCount = std::size(signalSemaphores);
	submitInfo.pSignalSemaphores = signalSemaphores;

	inFlightFence.reset();
	
	device->graphicsQueue().submit({ submitInfo }, inFlightFence.handle.get());

	vk::SwapchainKHR swapchains[] = { swapchain->handle };
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = std::size(signalSemaphores);
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = std::size(swapchains);
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	device->presentQueue().presentKHR(presentInfo);

	if (m_result == vk::Result::eErrorOutOfDateKHR)
	{
		recreateSwapchain();
		return;
	}

	if (m_result != vk::Result::eSuccess)
	{
		throw HanoException(std::string("failed to present next image (") + to_string(m_result) + ")");
	}

	m_currentFrame = (m_currentFrame + 1) % m_inFlightFences.size();
}

vkh::FrameBuffer const& VulkanContext::getCurrentFrameBuffer() const
{
	return swapchainFrameBuffers[imageIndex];
}

uint32 VulkanContext::getCurrentImageIndex() const noexcept
{
	return imageIndex;
}
