#include <vulkanContext.hpp>
#include <renderer/scene.hpp>
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
	device = std::make_unique<vkh::Device>(getSuitableDevice(), *surface, std::vector(c_vulkanDefaultDeviceRequiredExtentions.begin(), c_vulkanDefaultDeviceRequiredExtentions.end()), vkAllocator);
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

void VulkanContext::createRtStructures(Scene const& scene)
{
	createAccelerationStructures(scene);
	createRaytracingDescriptorSets(scene);
}
//@Review
void VulkanContext::createAccelerationStructures(Scene const& scene)
{
	auto nbModels = scene.getModels().size();
	std::vector<vkh::GeometryInstance> instances;
	m_bottomLevelAccelerationStructures.reserve(nbModels);
	
	uint32 i = 0;
	// create one GeometryInstance per mesh, this is not the best way of doing this, but it should be the easiest way
	for (auto const& model : scene.getModels())
	{
		auto& bottomAs = m_bottomLevelAccelerationStructures.emplace_back(*device, std::vector{ model.get().getMesh().toVkGeometryNV() }, true);
		{
			vkh::SingleTimeCommands command(*commandPool);
			bottomAs.generate(command.buffer());
		}
		
		instances.push_back(vkh::TopLevelAS::createGeometryInstance(bottomAs, model.get().transform.getMatrix(), i, 0 /*@TODO*/));
	}

	m_topLevelAccelerationStructure = std::make_unique<vkh::TopLevelAS>(*device, instances, true);
	{
		vkh::SingleTimeCommands command(*commandPool);
		m_topLevelAccelerationStructure->generate(command.buffer());
	}
}

void VulkanContext::createRaytracingOutImage()
{
	m_rtOutputImage.init(*device, swapchain->extent, swapchain->format, vk::MemoryPropertyFlagBits::eDeviceLocal,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eColorAttachment
		| vk::ImageUsageFlagBits::eSampled
		| vk::ImageUsageFlagBits::eStorage
		| vk::ImageUsageFlagBits::eTransferSrc);

	vkh::setObjectName(m_rtOutputImage, "rtOutputImage");

	m_rtOutputImageView.init(*device, m_rtOutputImage.handle.get(), swapchain->format, vk::ImageAspectFlagBits::eColor);
}

// @Review
void VulkanContext::createRaytracingDescriptorSets(Scene const& scene)
{
	std::vector<vkh::DescriptorBinding> descriptorBindings;
	descriptorBindings.push_back({ 0, 1, vk::DescriptorType::eAccelerationStructureNV, vk::ShaderStageFlagBits::eRaygenNV }); // TLAS
	descriptorBindings.push_back({ 1, 1, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eRaygenNV}); // outputImage
	descriptorBindings.push_back({ 2, 1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eRaygenNV }); // camera matrices
	m_rtDescriptorPool = std::make_unique<vkh::DescriptorPool>(*device, descriptorBindings, /*@TODO*/500);
	m_rtDescriptorSetLayout.init(*device, descriptorBindings);
	m_rtDescriptorSets.init(*m_rtDescriptorPool, m_rtDescriptorSetLayout, 1); // @TODO
	
	vk::WriteDescriptorSetAccelerationStructureNV accelWrite;
	accelWrite.accelerationStructureCount = 1;
	accelWrite.pAccelerationStructures = &m_topLevelAccelerationStructure->handle.get();
	m_rtDescriptorSets.push(0, 0, accelWrite);
	
	vk::DescriptorImageInfo imageInfo;
	//imageInfo.imageLayout = m_rtOutputImage.imageLayout;
	imageInfo.imageView = m_rtOutputImageView.handle.get();
	imageInfo.imageLayout = vk::ImageLayout::eGeneral;
	imageInfo.sampler = vk::Sampler();
	m_rtDescriptorSets.push(0, 1, imageInfo);
	
	// @TODO redo
	CameraMatrices camMtr = { .view = scene.camera.viewMtr, .proj = scene.camera.projectionMtr };
	m_cameraUbo.init(*device, sizeof(CameraMatrices), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	vkh::setObjectName(m_cameraUbo, "camera UBO");

	void* camData = m_cameraUbo.map();
		memcpy(camData, &camMtr, sizeof(CameraMatrices));
	m_cameraUbo.unMap();
	
	vk::DescriptorBufferInfo cameraInfo;
	cameraInfo.buffer = m_cameraUbo.handle.get();
	cameraInfo.offset = 0;
	cameraInfo.range = VK_WHOLE_SIZE;
	m_rtDescriptorSets.push(0, 2, cameraInfo);

	m_rtDescriptorSets.updateDescriptors(0);
}

// should be called when resizing window after created a new image with correct size
void VulkanContext::updateRaytracingOutImage()
{
	vk::DescriptorImageInfo imageInfo;
	//imageInfo.imageLayout = m_rtOutputImage.imageLayout;
	imageInfo.imageLayout = vk::ImageLayout::eGeneral;
	imageInfo.imageView = m_rtOutputImageView.handle.get();
	imageInfo.sampler = vk::Sampler();
	m_rtDescriptorSets.push(0, 1, imageInfo);

	m_rtDescriptorSets.updateDescriptors(0);
}

void VulkanContext::createRaytracingPipeline()
{
	vkh::RaytracingPipelineGenerator pipelineGen;
	vkh::ShaderModule raygen(*device, "assets/shaders/raygen.spv", vk::ShaderStageFlagBits::eRaygenNV);
	vkh::ShaderModule closestHit(*device, "assets/shaders/closestHit.spv", vk::ShaderStageFlagBits::eClosestHitNV);
	vkh::ShaderModule miss(*device, "assets/shaders/miss.spv", vk::ShaderStageFlagBits::eMissNV);

	pipelineGen.addHitGroup({ raygen }, vk::RayTracingShaderGroupTypeNV::eGeneral);
	pipelineGen.addHitGroup({ miss }, vk::RayTracingShaderGroupTypeNV::eGeneral);
	pipelineGen.addHitGroup({ closestHit }, vk::RayTracingShaderGroupTypeNV::eTrianglesHitGroup);
	m_rtPipeline = pipelineGen.create(m_rtDescriptorSetLayout, /*@TODO Recursion*/1);
}

// @Review
void VulkanContext::createShaderBindingTable()
{
	m_sbt = std::make_unique<vkh::ShaderBindingTable>(*device, m_rtPipeline, 
		std::vector{ vkh::ShaderBindingTable::Entry{ 0, { } } }, // raygenEntries
		std::vector{ vkh::ShaderBindingTable::Entry{ 1, { } } }, // miss
		std::vector{ vkh::ShaderBindingTable::Entry{ 2, { } } }  // hit
		);
}

// @Review
void VulkanContext::raytrace(vk::CommandBuffer commandBuffer)
{
	vk::ImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;

	m_rtOutputImage.insertBarrier(commandBuffer, subresourceRange, vk::AccessFlagBits(), vk::AccessFlagBits::eShaderWrite, vk::ImageLayout::eGeneral);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eRayTracingNV, m_rtPipeline.handle.get());
	m_rtDescriptorSets.bindDescriptor(0, commandBuffer, vk::PipelineBindPoint::eRayTracingNV, m_rtPipeline.pipelineLayout.get());
	auto const& sbtBuffer = m_sbt->getBuffer().handle.get();
	commandBuffer.traceRaysNV(sbtBuffer, m_sbt->rayGenOffset(),
		sbtBuffer, m_sbt->missOffset(), m_sbt->missEntrySize(),
		sbtBuffer, m_sbt->hitGroupOffset(), m_sbt->hitGroupEntrySize(),
		sbtBuffer, 0, 0,
		swapchain->extent.width, swapchain->extent.height, // imageExtent
		1); //depth

	m_rtOutputImage.insertBarrier(commandBuffer, subresourceRange, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eTransferSrcOptimal);
	
	vk::ImageMemoryBarrier barrier;
	barrier.srcAccessMask = vk::AccessFlagBits();
	barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.oldLayout = vk::ImageLayout::eUndefined;
	barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = swapchain->images[imageIndex];
	barrier.subresourceRange = subresourceRange;

	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags(),
		{}, {}, { barrier });

	vk::ImageCopy copyRegion;
	copyRegion.srcSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };
	copyRegion.srcOffset = { 0, 0, 0 };
	copyRegion.dstSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };
	copyRegion.dstOffset = { 0, 0, 0 };
	copyRegion.extent = { swapchain->extent.width, swapchain->extent.height, 1 };

	commandBuffer.copyImage(m_rtOutputImage.handle.get(), vk::ImageLayout::eTransferSrcOptimal,
		swapchain->images[imageIndex], vk::ImageLayout::eTransferDstOptimal, copyRegion);

	barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.dstAccessMask = vk::AccessFlagBits();
	barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	barrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = swapchain->images[imageIndex];
	barrier.subresourceRange = subresourceRange;

	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags(),
		{}, {}, { barrier });
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

void VulkanContext::destroy()
{
	if (!device)
		return;

	device->handle.waitIdle();

	m_topLevelAccelerationStructure.reset();
	m_bottomLevelAccelerationStructures.clear();
	m_rtPipeline.pipelineLayout.reset();
	m_rtPipeline.handle.reset();
	m_sbt.reset();
	m_cameraUbo.destroy();
	m_rtDescriptorSetLayout.destroy();
	m_rtDescriptorPool.reset();
	m_rtOutputImageView.destroy();
	m_rtOutputImage.destroy();

	deleteSwapchain();
	descriptorPool.reset();
	commandPool.reset();
	device.reset();
	surface.reset();
	instance.reset();
}