#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhRenderPass.hpp"

namespace hano::vkh
{
	struct Swapchain;
	struct DescriptorSetManager;

	struct GraphicsPipeline
	{

		GraphicsPipeline(	Swapchain const& swapChain,
							DepthBuffer const& depthBuffer,
							bool isWireFrame);

		vk::UniquePipeline handle;
		vkh::RenderPass renderPass;
		vk::UniquePipelineLayout pipelineLayout;
		std::unique_ptr<DescriptorSetManager> descriptorSetManager;

		Swapchain const* swapchain;
		bool const isWireframe;
	};
}