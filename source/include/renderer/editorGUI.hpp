#pragma once

#include <memory>
#include <functional>
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

			void render(vk::CommandBuffer commandBuffer, vkh::FrameBuffer const& framebuffer);
			void handleSwapchainRecreation();

			std::function<void()> onGUI;

		private:

			VulkanContext const* vkContext;
			std::unique_ptr<vkh::DescriptorPool> descriptorPool;
			std::unique_ptr<vkh::RenderPass> renderPass;
	};
}