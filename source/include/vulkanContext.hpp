#pragma once

#define WIN32_LEAN_AND_MEAN // avoid unecessary includes and macros
#include <memory>
#include <optional>
#include <functional>
#include <vector>
#include <glfw/glfw3.h>
#include "vulkanHelpers.hpp"
#include "renderer/texture.hpp"

namespace hano
{
	struct Scene;

	struct VulkanConfig
	{
		const char* appName;
		const char* engineName;
		bool vsyncEnabled;
	};

	//@Review
	class VulkanContext
	{

		public:
		
			static constexpr size_t c_maxFramesInFlight = 2;

			void init(GLFWwindow* window, VulkanConfig const& config);
			void destroy();

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
			void createSceneBuffers();
			void createRaytracingPipeline();
			void createShaderBindingTable();

			void updateTlas(vk::CommandBuffer commandBuffer);
			void updateRtDescriptorSets(Scene const& scene);

			void raytrace(vk::CommandBuffer commandBuffer);
			void recreateRtPipelineObjects(); // recreate all object dependent of the pipeline
			void reloadShaders();
			// ------ 

			void createEnvMap();

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
		
			Scene* scene;

		protected:
			
			uint64 m_currentFrame = 0;
			uint32 swapchainImageIndex = 0;
			vk::CommandBuffer commandBuffer;
			vk::Result m_result;

			GLFWwindow* m_window = nullptr;
			std::vector<vkh::Semaphore> m_imageAvailableSemaphores;
			std::vector<vkh::Semaphore> m_renderFinishedSemaphores;
			std::vector<vkh::Fence> m_inFlightFences;

			bool m_shaderReloadingAsked = false;

			// rtx
			std::unique_ptr<vkh::TopLevelAS> m_topLevelAccelerationStructure;
			std::vector<vkh::BottomLevelAS> m_bottomLevelAccelerationStructures;
			vkh::RaytracingPipeline m_rtPipeline;
			std::unique_ptr<vkh::ShaderBindingTable> m_sbt;

			std::unique_ptr<vkh::DescriptorPool> m_rtDescriptorPool;
			vkh::DescriptorSetLayout m_rtDescriptorSetLayout;
			vkh::DescriptorSets m_rtDescriptorSets;

			std::vector<vkh::Image> m_rtOutputImages;
			std::vector<vkh::ImageView> m_rtOutputImageViews;

			// scene
			// buffers that store all the vertices and indices of the scene
			hano::Texture m_envMap;
			vkh::Buffer m_cameraUbo;
			vkh::Buffer m_sceneVertexBuffer;
			vkh::Buffer m_sceneIndexBuffer;
			vkh::Buffer m_sceneOffsetsBuffer;
			vkh::Buffer m_lightBuffer;
	};
}