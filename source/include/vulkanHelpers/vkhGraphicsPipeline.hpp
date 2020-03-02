#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhDescriptorSetManager.hpp"
#include "vkhRenderPass.hpp"

namespace hano::vkh
{
	struct Swapchain;

	struct GraphicsPipeline
	{

		GraphicsPipeline(	Swapchain const& swapChain,
							DepthBuffer const& depthBuffer,
							bool isWireFrame);

		Swapchain const* swapchain;
		vkh::RenderPass renderPass;
		std::unique_ptr<vkh::DescriptorSetManager> descriptorSetManager;
		vk::UniquePipelineLayout pipelineLayout;
		vk::UniquePipeline handle;

		bool const isWireframe;
	};
}