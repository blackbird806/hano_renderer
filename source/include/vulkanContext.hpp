#pragma once

#define WIN32_LEAN_AND_MEAN // avoid unecessary includes and macros
#include <memory>
#include <optional>
#include <functional>
#include <glfw/glfw3.h>
#include "vulkanHelpers.hpp"

namespace hano
{
	struct VulkanConfig
	{
		const char* appName;
		const char* engineName;
		bool vsyncEnabled;
	};

	struct VulkanContext
	{
		static vkh::Device& getDevice();
		static vkh::CommandPool& getCommandPool();

		void init(GLFWwindow* window, VulkanConfig const& config);

		std::optional<vk::CommandBuffer> beginFrame();
		void endFrame();

		void createSwapchain();
		void deleteSwapchain();
		void recreateSwapchain();

		vkh::FrameBuffer const& getCurrentFrameBuffer() const;

		std::vector<vk::PhysicalDevice> const& getPhysicalDevices() const;
		std::vector<vk::ExtensionProperties> const& getExtensions() const;
		vk::PhysicalDevice getSuitableDevice() const;
		uint32 getCurrentImageIndex() const noexcept;

		vk::AllocationCallbacks* vkAllocator = nullptr;
		std::unique_ptr<vkh::Instance> instance;
		std::unique_ptr<vkh::Surface> surface;
		std::unique_ptr<vkh::Device> device;
		std::unique_ptr<vkh::Swapchain> swapchain;
		std::vector<vkh::FrameBuffer> swapchainFrameBuffers;
		std::unique_ptr<vkh::CommandPool> commandPool;
		std::unique_ptr<vkh::CommandBuffers> commandBuffers;
		std::unique_ptr<vkh::DepthBuffer> depthBuffer;
		std::unique_ptr<vkh::GraphicsPipeline> graphicsPipeline;

		bool frameBufferResized = false;
		std::function<void()> onRecreateSwapchain;

		private:
			
			uint64 m_currentFrame = 0;
			uint32 imageIndex = 0;
			vk::CommandBuffer commandBuffer;
			vk::Result m_result;

			GLFWwindow* m_window = nullptr;
			std::vector<vkh::Semaphore> m_imageAvailableSemaphores;
			std::vector<vkh::Semaphore> m_renderFinishedSemaphores;
			std::vector<vkh::Fence> m_inFlightFences;

			static vkh::Device* global_device;
			static vkh::CommandPool* global_commandPool;
	};
}