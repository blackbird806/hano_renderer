#pragma once

#define WIN32_LEAN_AND_MEAN // avoid unecessary includes and macros
#include <memory>
#include <optional>
#include <functional>
#include <vector>
#include <glfw/glfw3.h>
#include "vulkanHelpers.hpp"

namespace hano
{
	struct Scene;

	struct VulkanConfig
	{
		const char* appName;
		const char* engineName;
		bool vsyncEnabled;
	};

	class VulkanContext
	{

	public:
		
		void init(GLFWwindow* window, VulkanConfig const& config);

		std::optional<vk::CommandBuffer> beginFrame();
		void endFrame();

		void createSwapchain();
		void deleteSwapchain();
		void recreateSwapchain();

		// raytracing @Review
		void createRtStructures(Scene const& scene);
		void createAccelerationStructures(Scene const& scene);
		void createRaytracingOutImage();
		void createRaytracingDescriptorSets(Scene const& scene);
		void updateRaytracingOutImage();
		void createRaytracingPipeline();
		void createShaderBindingTable();
		void raytrace(vk::CommandBuffer commandBuffer);
		// -- 

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
		std::unique_ptr<vkh::DescriptorPool> descriptorPool;

		bool frameBufferResized = false;
		std::function<void()> onRecreateSwapchain;
		
		protected:
			
			uint64 m_currentFrame = 0;
			uint32 imageIndex = 0;
			vk::CommandBuffer commandBuffer;
			vk::Result m_result;

			GLFWwindow* m_window = nullptr;
			std::vector<vkh::Semaphore> m_imageAvailableSemaphores;
			std::vector<vkh::Semaphore> m_renderFinishedSemaphores;
			std::vector<vkh::Fence> m_inFlightFences;

			// rtx
			std::unique_ptr<vkh::TopLevelAS> m_topLevelAccelerationStructure;
			std::vector<vkh::BottomLevelAS> m_bottomLevelAccelerationStructures;
			vkh::RaytracingPipeline m_rtPipeline;
			std::unique_ptr<vkh::ShaderBindingTable> m_sbt;
			vkh::Buffer m_cameraUbo;

			std::unique_ptr<vkh::DescriptorPool> m_rtDescriptorPool;
			vkh::DescriptorSetLayout m_rtDescriptorSetLayout;
			vkh::DescriptorSets m_rtDescriptorSets;

			vkh::Image m_rtOutputImage;
			vkh::ImageView m_rtOutputImageView;
	};
}