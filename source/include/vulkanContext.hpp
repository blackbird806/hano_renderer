#pragma once

#include <memory>
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

		std::vector<vk::PhysicalDevice> const& getPhysicalDevices() const;
		std::vector<vk::ExtensionProperties> const& getExtensions() const;
		vk::PhysicalDevice getSuitableDevice() const;

		vk::AllocationCallbacks* vkAllocator = nullptr;
		std::unique_ptr<vkh::Instance> instance;
		std::unique_ptr<vkh::Surface> surface;
		std::unique_ptr<vkh::Device> device;
		std::unique_ptr<vkh::Swapchain> swapchain;
		std::unique_ptr<vkh::CommandPool> commandPool;
		std::unique_ptr<vkh::CommandBuffers> commandBuffers;
		std::unique_ptr<vkh::DepthBuffer> depthBuffer;

		private:
			static vkh::Device* global_device;
			static vkh::CommandPool* global_commandPool;
	};
}