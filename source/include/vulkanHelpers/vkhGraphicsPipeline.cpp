#include <vulkanHelpers/vkhGraphicsPipeline.hpp>
#include <iterator>
#include <vulkanHelpers/vkhSwapchain.hpp>
#include <vulkanHelpers/vkhRenderPass.hpp>
#include <vulkanHelpers/vkhDepthBuffer.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhDescriptorBinding.hpp>
#include <vulkanHelpers/vkhDescriptorSetManager.hpp>
#include <vulkanHelpers/vkhShaderModule.hpp>
#include <renderer/vertex.hpp>

using namespace hano::vkh;

GraphicsPipeline::GraphicsPipeline(Swapchain const& swapChain_, DepthBuffer const& depthBuffer, bool isWireframe_)
    : swapchain(&swapChain_), renderPass(*swapchain, depthBuffer, true, true), isWireframe(isWireframe_)
{
	auto const& device = swapchain->device;
	auto const bindingDescription = Vertex::getBindingDescription();
	auto const attributeDescriptions = Vertex::getAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	vk::Viewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain->extent.width);
	viewport.height = static_cast<float>(swapchain->extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vk::Rect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain->extent;

	vk::PipelineViewportStateCreateInfo viewportState = {};
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	vk::PipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = isWireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	vk::PipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	vk::PipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = vk::CompareOp::eLess;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

	vk::PipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	// Create descriptor pool/sets.
	std::vector<DescriptorBinding> descriptorBindings =
	{
		{0, 1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex},
		{1, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
		{2, 1 , vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}
	};

	descriptorSetManager = std::make_unique<DescriptorSetManager>(device, descriptorBindings, 8192);

	auto& descriptorSets = descriptorSetManager->descriptorSets;

	// Create pipeline layout and render pass.

	vk::DescriptorSetLayout descriptorSetLayouts[] = { descriptorSetManager->descriptorSetLayout->handle.get()};

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.setLayoutCount = std::size(descriptorSetLayouts);
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	pipelineLayout = device.handle.createPipelineLayoutUnique(pipelineLayoutInfo, device.allocator);

	// Load shaders.
	assert(false); // TODO shaders
	ShaderModule const vertShader(device, "../assets/shaders/Graphics.vert.spv");
	ShaderModule const fragShader(device, "../assets/shaders/Graphics.frag.spv");

	vk::PipelineShaderStageCreateInfo shaderStages[] =
	{
		vertShader.createShaderStageInfo(vk::ShaderStageFlagBits::eVertex),
		fragShader.createShaderStageInfo(vk::ShaderStageFlagBits::eFragment)
	};

	// Create graphic pipeline
	vk::GraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.basePipelineHandle = nullptr; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional
	pipelineInfo.layout = pipelineLayout.get();
	pipelineInfo.renderPass = renderPass.handle.get();
	pipelineInfo.subpass = 0;

	handle = device.handle.createGraphicsPipelineUnique(nullptr, { pipelineInfo }, device.allocator);
}
