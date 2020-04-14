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
	m_lightBuffers.clear();
	m_cameraUbos.clear();
	m_rtOutputImageViews.clear();
	m_rtOutputImages.clear();

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
	createRaytracingOutImage();
	m_rtDescriptorSets.destroy();
	createRaytracingDescriptorSets(*scene);
	onRecreateSwapchain();
}

void VulkanContext::createRtStructures(Scene const& scene_)
{
	createAccelerationStructures(scene_);
	createRaytracingDescriptorSets(scene_);
}

//@Review
void VulkanContext::createAccelerationStructures(Scene const& scene_)
{
	auto nbModels = scene_.getModels().size();
	std::vector<vkh::GeometryInstance> instances;
	m_bottomLevelAccelerationStructures.reserve(nbModels);
	
	// create one GeometryInstance per mesh, this is not the best way of doing this, but it should be the easiest way
	for (uint32 i = 0;auto const& model : scene_.getModels())
	{
		auto& bottomAs = m_bottomLevelAccelerationStructures.emplace_back(*device, std::vector{ model.get().getMesh().toVkGeometryNV() }, true);
		{
			vkh::SingleTimeCommands command(*commandPool);
			bottomAs.generate(command.buffer());
		}
		
		instances.push_back(vkh::TopLevelAS::createGeometryInstance(bottomAs, model.get().transform.getMatrix(), i, 0 /*@TODO*/));
		i++;
	}

	m_topLevelAccelerationStructure = std::make_unique<vkh::TopLevelAS>(*device, instances.size(), true);
	{
		vkh::SingleTimeCommands command(*commandPool);
		m_topLevelAccelerationStructure->generate(command.buffer(), instances);
	}
}

void VulkanContext::createRaytracingOutImage()
{
	m_rtOutputImages.resize(c_maxFramesInFlight);
	m_rtOutputImageViews.resize(c_maxFramesInFlight);
	for (int i = 0; i < c_maxFramesInFlight; i++)
	{
		m_rtOutputImages[i].init(*device, swapchain->extent, swapchain->format, vk::MemoryPropertyFlagBits::eDeviceLocal,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eColorAttachment
			| vk::ImageUsageFlagBits::eSampled
			| vk::ImageUsageFlagBits::eStorage
			| vk::ImageUsageFlagBits::eTransferSrc);

		vkh::setObjectName(m_rtOutputImages[i], "rtOutputImage");

		m_rtOutputImageViews[i].init(*device, m_rtOutputImages[i].handle.get(), swapchain->format, vk::ImageAspectFlagBits::eColor);
	}
}

// @Review
void VulkanContext::createRaytracingDescriptorSets(Scene const& scene_)
{
	std::vector<vkh::DescriptorBinding> descriptorBindings;

	uint32 binding = 0;
	descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eAccelerationStructureNV, vk::ShaderStageFlagBits::eRaygenNV | vk::ShaderStageFlagBits::eClosestHitNV }); // TLAS
	descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eRaygenNV }); // outputImage
	descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eRaygenNV }); // camera matrices
	descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eClosestHitNV }); // vertex Buffer
	descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eClosestHitNV }); // index Buffer
	descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eClosestHitNV }); // index Buffer
	descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eClosestHitNV }); // light Buffers
	descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eMissNV }); // envmap texture
	//descriptorBindings.push_back({ binding++, 1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eClosestHitNV }); // textures

	m_rtDescriptorPool = std::make_unique<vkh::DescriptorPool>(*device, descriptorBindings, /*@TODO*/500);
	m_rtDescriptorSetLayout.init(*device, descriptorBindings);
	m_rtDescriptorSets.init(*m_rtDescriptorPool, m_rtDescriptorSetLayout, c_maxFramesInFlight);
	m_cameraUbos.resize(c_maxFramesInFlight);
	m_lightBuffers.resize(c_maxFramesInFlight);

	for (int i = 0; i < c_maxFramesInFlight; i++)
	{
		vk::WriteDescriptorSetAccelerationStructureNV accelWrite;
		accelWrite.accelerationStructureCount = 1;
		accelWrite.pAccelerationStructures = &m_topLevelAccelerationStructure->handle.get();
		m_rtDescriptorSets.push(i, 0, accelWrite);
	
		vk::DescriptorImageInfo imageInfo;
		imageInfo.imageView = m_rtOutputImageViews[i].handle.get();
		imageInfo.imageLayout = vk::ImageLayout::eGeneral;
		imageInfo.sampler = vk::Sampler();
		m_rtDescriptorSets.push(i, 1, imageInfo);
	
		CameraMatrices camMtr = { .view = scene_.camera.viewMtr, .proj = scene_.camera.projectionMtr };
		m_cameraUbos[i].init(*device, sizeof(CameraMatrices), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		vkh::setObjectName(m_cameraUbos[i], "camera UBO");

		void* camData = m_cameraUbos[i].map();
			memcpy(camData, &camMtr, sizeof(CameraMatrices));
		m_cameraUbos[i].unMap();

		vk::DescriptorBufferInfo cameraInfo;
		cameraInfo.buffer = m_cameraUbos[i].handle.get();
		cameraInfo.offset = 0;
		cameraInfo.range = VK_WHOLE_SIZE;
		m_rtDescriptorSets.push(i, 2, cameraInfo);

		vk::DescriptorBufferInfo verticesInfo;
		verticesInfo.buffer = m_sceneVertexBuffers[i].handle.get();
		verticesInfo.offset = 0;
		verticesInfo.range = VK_WHOLE_SIZE;
		m_rtDescriptorSets.push(i, 3, verticesInfo);

		vk::DescriptorBufferInfo indicesInfo;
		indicesInfo.buffer = m_sceneIndexBuffers[i].handle.get();
		indicesInfo.offset = 0;
		indicesInfo.range = VK_WHOLE_SIZE;
		m_rtDescriptorSets.push(i, 4, indicesInfo);

		vk::DescriptorBufferInfo offsetsInfo;
		offsetsInfo.buffer = m_sceneOffsetsBuffers[i].handle.get();
		offsetsInfo.offset = 0;
		offsetsInfo.range = VK_WHOLE_SIZE;
		m_rtDescriptorSets.push(i, 5, offsetsInfo);

		m_lightBuffers[i].init(*device, sizeof(PointLight) * Scene::c_maxLights, 
			vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		vkh::setObjectName(m_lightBuffers[i], "light");

		void* lightData = m_lightBuffers[i].map();
			memcpy(lightData, scene_.getPointLights().data(), scene_.getPointLights().size() * sizeof(PointLight));
		m_lightBuffers[i].unMap();

		vk::DescriptorBufferInfo lightInfo;
		lightInfo.buffer = m_lightBuffers[i].handle.get();
		lightInfo.offset = 0;
		lightInfo.range = VK_WHOLE_SIZE;
		m_rtDescriptorSets.push(i, 6, lightInfo);

		vk::DescriptorImageInfo envMapInfo;
		envMapInfo.imageLayout = m_envMap.image.imageLayout;
		envMapInfo.imageView = m_envMap.imageView.handle.get();
		envMapInfo.sampler = m_envMap.sampler.get();
		m_rtDescriptorSets.push(i, 7, envMapInfo);

		m_rtDescriptorSets.updateDescriptors(i);
	}
}

void VulkanContext::createSceneBuffers()
{
	// Concatenate all the models
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<glm::uvec2> offsets;

	for (auto const& model : scene->getModels())
	{
		auto const vertexOffset = vertices.size();
		auto const indexOffset = indices.size();

		auto const& mesh = model.get().getMesh();
		vertices.insert(vertices.end(), mesh.getVertices().begin(), mesh.getVertices().end());
		indices.insert(indices.end(), mesh.getIndices().begin(), mesh.getIndices().end());
		offsets.emplace_back(vertexOffset, indexOffset);
	}

	m_sceneVertexBuffers.reserve(c_maxFramesInFlight);
	for (int i = 0; i < c_maxFramesInFlight; i++)
	{
		auto& vbuff = m_sceneVertexBuffers.emplace_back(*device, sizeof(vertices[0]) * vertices.size(), 
			vk::BufferUsageFlagBits::eStorageBuffer, 
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		void* data = vbuff.map();

			memcpy(data, vertices.data(), vbuff.getSize());

		vbuff.unMap();

		auto& ibuff = m_sceneIndexBuffers.emplace_back(*device, sizeof(indices[0]) * indices.size(),
			vk::BufferUsageFlagBits::eStorageBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		data = ibuff.map();

			memcpy(data, indices.data(), ibuff.getSize());

		ibuff.unMap();

		auto& obuff = m_sceneOffsetsBuffers.emplace_back(*device, sizeof(offsets[0]) * offsets.size(),
			vk::BufferUsageFlagBits::eStorageBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		data = obuff.map();

			memcpy(data, offsets.data(), obuff.getSize());

		obuff.unMap();
	}
}

void VulkanContext::updateTlas(vk::CommandBuffer commandBuffer)
{
	auto const nbModels = scene->getModels().size();
	std::vector<vkh::GeometryInstance> instances(nbModels);
	for (uint32 i = 0; i < nbModels; i++)
	{
		instances[i] = vkh::TopLevelAS::createGeometryInstance(m_bottomLevelAccelerationStructures[i], 
			scene->getModels()[i].get().transform.getMatrix(), i, 0 /*@TODO*/);
	}

	m_topLevelAccelerationStructure->update(commandBuffer, instances);
}

void VulkanContext::updateRtDescriptorSets(Scene const& scene_)
{
	// update camera
	CameraMatrices camMtr = { .view = scene_.camera.viewMtr, .proj = scene_.camera.projectionMtr };

	void* camData = m_cameraUbos[m_currentFrame].map();
		memcpy(camData, &camMtr, sizeof(CameraMatrices));
	m_cameraUbos[m_currentFrame].unMap();

	vk::DescriptorBufferInfo cameraInfo;
	cameraInfo.buffer = m_cameraUbos[m_currentFrame].handle.get();
	cameraInfo.offset = 0;
	cameraInfo.range = VK_WHOLE_SIZE;
	m_rtDescriptorSets.push(m_currentFrame, 2, cameraInfo);

	vk::WriteDescriptorSetAccelerationStructureNV accelWrite;
	accelWrite.accelerationStructureCount = 1;
	accelWrite.pAccelerationStructures = &m_topLevelAccelerationStructure->handle.get();
	m_rtDescriptorSets.push(m_currentFrame, 0, accelWrite);

	void* lightData = m_lightBuffers[m_currentFrame].map();
		memcpy(lightData, scene_.getPointLights().data(), scene_.getPointLights().size() * sizeof(PointLight));
	m_lightBuffers[m_currentFrame].unMap();

	vk::DescriptorBufferInfo lightInfo;
	lightInfo.buffer = m_lightBuffers[m_currentFrame].handle.get();
	lightInfo.offset = 0;
	lightInfo.range = VK_WHOLE_SIZE;
	m_rtDescriptorSets.push(m_currentFrame, 6, lightInfo);

	m_rtDescriptorSets.updateDescriptors(m_currentFrame);
}

void VulkanContext::createRaytracingPipeline()
{
	vkh::RaytracingPipelineGenerator pipelineGen;
	vkh::ShaderModule raygen(*device, "assets/shaders/raygen.spv", vk::ShaderStageFlagBits::eRaygenNV);
	vkh::ShaderModule closestHit(*device, "assets/shaders/closestHit.spv", vk::ShaderStageFlagBits::eClosestHitNV);
	vkh::ShaderModule miss(*device, "assets/shaders/miss.spv", vk::ShaderStageFlagBits::eMissNV);
	vkh::ShaderModule shadowMiss(*device, "assets/shaders/shadowMiss.spv", vk::ShaderStageFlagBits::eMissNV);

	pipelineGen.addHitGroup({ raygen }, vk::RayTracingShaderGroupTypeNV::eGeneral);
	pipelineGen.addHitGroup({ miss }, vk::RayTracingShaderGroupTypeNV::eGeneral);
	pipelineGen.addHitGroup({ shadowMiss }, vk::RayTracingShaderGroupTypeNV::eGeneral);
	pipelineGen.addHitGroup({ closestHit }, vk::RayTracingShaderGroupTypeNV::eTrianglesHitGroup);

	vk::PushConstantRange nbLightsConst;
	nbLightsConst.offset = 0;
	nbLightsConst.stageFlags = vk::ShaderStageFlagBits::eClosestHitNV;
	nbLightsConst.size = sizeof(uint32);

	m_rtPipeline = pipelineGen.create(m_rtDescriptorSetLayout, { nbLightsConst }, /*@TODO Recursion*/2);
}

// @Review
void VulkanContext::createShaderBindingTable()
{
	m_sbt = std::make_unique<vkh::ShaderBindingTable>(*device, m_rtPipeline, 
		std::vector{ vkh::ShaderBindingTable::Entry{ 0, { } } },	// raygenEntries
		std::vector{ vkh::ShaderBindingTable::Entry{ 1, { } }, vkh::ShaderBindingTable::Entry{ 2, { } } },	// miss
		std::vector{ vkh::ShaderBindingTable::Entry{ 3, { } } }		// hit
		);
}

// @Review
void VulkanContext::raytrace(vk::CommandBuffer commandBuffer)
{
	updateRtDescriptorSets(*scene);

	vk::ImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;

	m_rtOutputImages[m_currentFrame].insertBarrier(commandBuffer, subresourceRange, vk::AccessFlagBits(), vk::AccessFlagBits::eShaderWrite, vk::ImageLayout::eGeneral);
	
	std::vector nbLightsConst = { (uint32)scene->getPointLights().size() };
	commandBuffer.pushConstants(m_rtPipeline.pipelineLayout.get(), vk::ShaderStageFlagBits::eClosestHitNV, 0, nbLightsConst.size() * sizeof(uint32), nbLightsConst.data());
	
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eRayTracingNV, m_rtPipeline.handle.get());
	m_rtDescriptorSets.bindDescriptor(m_currentFrame, commandBuffer, vk::PipelineBindPoint::eRayTracingNV, m_rtPipeline.pipelineLayout.get());
	auto const& sbtBuffer = m_sbt->getBuffer().handle.get();
	commandBuffer.traceRaysNV(sbtBuffer, m_sbt->rayGenOffset(),
		sbtBuffer, m_sbt->missOffset(), m_sbt->missEntrySize(),
		sbtBuffer, m_sbt->hitGroupOffset(), m_sbt->hitGroupEntrySize(),
		sbtBuffer, 0, 0,
		swapchain->extent.width, swapchain->extent.height, // imageExtent
		1); //depth

	m_rtOutputImages[m_currentFrame].insertBarrier(commandBuffer, subresourceRange, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eTransferSrcOptimal);
	
	vk::ImageMemoryBarrier barrier;
	barrier.srcAccessMask = vk::AccessFlagBits();
	barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.oldLayout = vk::ImageLayout::eUndefined;
	barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = swapchain->images[swapchainImageIndex];
	barrier.subresourceRange = subresourceRange;

	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags(),
		{}, {}, { barrier });

	vk::ImageCopy copyRegion;
	copyRegion.srcSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };
	copyRegion.srcOffset = { 0, 0, 0 };
	copyRegion.dstSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };
	copyRegion.dstOffset = { 0, 0, 0 };
	copyRegion.extent = { swapchain->extent.width, swapchain->extent.height, 1 };

	commandBuffer.copyImage(m_rtOutputImages[m_currentFrame].handle.get(), vk::ImageLayout::eTransferSrcOptimal,
		swapchain->images[swapchainImageIndex], vk::ImageLayout::eTransferDstOptimal, copyRegion);

	barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.dstAccessMask = vk::AccessFlagBits();
	barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	barrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = swapchain->images[swapchainImageIndex];
	barrier.subresourceRange = subresourceRange;

	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags(),
		{}, {}, { barrier });
}

void VulkanContext::recreateRtPipelineObjects()
{
	device->handle.waitIdle();
	m_rtPipeline.destroy();
	m_sbt.reset();
	createRaytracingPipeline();
	createShaderBindingTable();
}

void VulkanContext::reloadShaders()
{
	m_shaderReloadingAsked = true;
}

void VulkanContext::createEnvMap()
{
	m_envMap.init(*this, "assets/textures/envmap.jpg");
}

std::optional<vk::CommandBuffer> VulkanContext::beginFrame()
{
	auto constexpr noTimeout = std::numeric_limits<uint64>::max();
	
	auto& inFlightFence = m_inFlightFences[m_currentFrame];
	auto imageAvailableSemaphore = m_imageAvailableSemaphores[m_currentFrame].handle.get();
	auto renderFinishedSemaphore = m_renderFinishedSemaphores[m_currentFrame].handle.get();

	inFlightFence.wait(noTimeout);

	auto result = device->handle.acquireNextImageKHR(swapchain->handle, noTimeout, imageAvailableSemaphore, nullptr);
	swapchainImageIndex = result.value;
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

	commandBuffer = commandBuffers->begin(swapchainImageIndex);

	return { commandBuffer };
}

void VulkanContext::endFrame()
{
	auto& inFlightFence = m_inFlightFences[m_currentFrame];
	auto imageAvailableSemaphore = m_imageAvailableSemaphores[m_currentFrame].handle.get();
	auto renderFinishedSemaphore = m_renderFinishedSemaphores[m_currentFrame].handle.get();

	commandBuffers->end(swapchainImageIndex);

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
	presentInfo.pImageIndices = &swapchainImageIndex;
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

	if (m_shaderReloadingAsked)
	{
		recreateRtPipelineObjects();
		m_shaderReloadingAsked = false;
	}
}

vkh::FrameBuffer const& VulkanContext::getCurrentFrameBuffer() const
{
	return swapchainFrameBuffers[swapchainImageIndex];
}

uint32 VulkanContext::getCurrentImageIndex() const noexcept
{
	return swapchainImageIndex;
}

void VulkanContext::destroy()
{
	if (!device)
		return;

	device->handle.waitIdle();

	m_envMap.destroy();
	m_topLevelAccelerationStructure.reset();
	m_bottomLevelAccelerationStructures.clear();
	m_rtPipeline.pipelineLayout.reset();
	m_rtPipeline.handle.reset();
	m_sbt.reset();
	m_rtDescriptorSetLayout.destroy();
	m_rtDescriptorPool.reset();

	deleteSwapchain();
	descriptorPool.reset();
	commandPool.reset();
	device.reset();
	surface.reset();
	instance.reset();
}