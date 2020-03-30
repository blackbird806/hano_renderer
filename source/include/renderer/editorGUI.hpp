#pragma once

#include <memory>
#include <functional>
#include <vulkanContext.hpp>
#include <vulkanHelpers/vkhFrameBuffer.hpp>
#include <vulkanHelpers/vkhDescriptorPool.hpp>
#include <vulkanHelpers/vkhRenderPass.hpp>

namespace hano
{
	// @Review
	class CustomEditorGUI
	{
	public:
		virtual ~CustomEditorGUI() {};

		virtual void initUI() {};
		virtual void drawUI() {};
	};

	class EditorGUI
	{
		public:

			EditorGUI(VulkanContext const& vkContext_);
			virtual ~EditorGUI();

			void setCustomEditor(CustomEditorGUI* customEditor);
			void render(vk::CommandBuffer commandBuffer, vkh::FrameBuffer const& framebuffer);
			void handleSwapchainRecreation();

		private:

			VulkanContext const* vkContext;
			CustomEditorGUI* customEditor;
			std::unique_ptr<vkh::DescriptorPool> descriptorPool;
			std::unique_ptr<vkh::RenderPass> renderPass;
	};
}