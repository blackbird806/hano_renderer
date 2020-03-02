#pragma once

#include <memory>
#include <vulkanContext.hpp>
#include <vulkanHelpers/vkhFrameBuffer.hpp>
#include <vulkanHelpers/vkhDescriptorPool.hpp>
#include <vulkanHelpers/vkhRenderPass.hpp>

namespace hano
{
	class EditorGUI
	{
		public:
			EditorGUI(VulkanContext const& vkContext_);
			~EditorGUI();

			void Render(vk::CommandBuffer commandBuffer, vkh::FrameBuffer const& framebuffer);

		private:

			VulkanContext const* vkContext;
			std::unique_ptr<vkh::DescriptorPool> descriptorPool;
			std::unique_ptr<vkh::RenderPass> renderPass;
	};
}