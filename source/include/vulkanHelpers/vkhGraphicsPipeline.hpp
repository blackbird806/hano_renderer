#pragma once

#include <vulkan/vulkan.hpp>
#include "vkhRenderPass.hpp"

namespace hano::vkh
{
	struct Swapchain;
	struct DepthBuffer;

	struct GraphicsPipeline
	{
		GraphicsPipeline(
			 Swapchain const& swapChain,
			 DepthBuffer const& depthBuffer,
			 std::vector<Assets::UniformBuffer> const& uniformBuffers,
			 Assets::Scene const& scene,
			bool isWireFrame);

		vk::UniquePipeline handle;
		vkh::RenderPass renderPass;
		vk::UniquePipelineLayout pipelineLayout;

		Swapchain const* swapchain;
		bool const isWireframe;
	};
}