#pragma once

#include <memory>
#include "vulkanHelper.hpp"

namespace hano
{
	class Renderer
	{
		public:

			Renderer();
			~Renderer();

			std::vector<vk::PhysicalDevice> const& getPhysicalDevices() const;
			std::vector<vk::ExtensionProperties> const& getExtensions() const;
			vk::PhysicalDevice getSuitableDevice() const;
		
		private:

			struct GLFWwindow* m_window;

			vk::AllocationCallbacks* m_vkAllocator = nullptr;
			std::unique_ptr<vkh::Instance> m_instance;
			std::unique_ptr<vkh::Surface> m_surface;
			std::unique_ptr<vkh::Device> m_device;
			std::unique_ptr<vkh::Swapchain> m_swapchain;
			std::unique_ptr<vkh::CommandPool> m_commandPool;
			std::unique_ptr<vkh::CommandBuffers> m_commandBuffers;
			std::unique_ptr<vkh::DepthBuffer> m_depthBuffer;
	};
}
