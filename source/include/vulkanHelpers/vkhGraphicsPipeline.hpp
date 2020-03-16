#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhDescriptorSetLayout.hpp"
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
		vkh::DescriptorSetLayout descriptorSetLayout;
		vk::UniquePipelineLayout pipelineLayout;
		vk::UniquePipeline handle;

		bool const isWireframe;
	};
}