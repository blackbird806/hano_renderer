#include <glfw/glfw3.h>
#include <hanoRenderer.hpp>
#include <core/logger.hpp>

using namespace hano;

void Renderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	app->vkContext.frameBufferResized = true;
}

Renderer::Renderer()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);

	glfwSetFramebufferSizeCallback(m_window, &framebufferResizeCallback);
	vkContext.init(m_window, { .appName = "hanoRtx", .engineName = "hanoRenderer" });
	editorGUI = std::make_unique<EditorGUI>(vkContext);
	
	vkContext.onRecreateSwapchain = [&]() {
		editorGUI->handleSwapchainRecreation();
	};

	m_isRunning = true;

	currentScene.meshes.emplace_back("assets/obj/cube.obj");
}

Renderer::~Renderer()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Renderer::renderFrame()
{
	// @Review
	m_isRunning = !((bool)glfwWindowShouldClose(m_window));
	glfwPollEvents();

	auto commandBuffer = vkContext.beginFrame();
	if (commandBuffer)
	{
		std::array<vk::ClearValue, 2> clearValues = {};
		auto e = std::array{ 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[0].color = { e };
		clearValues[1].depthStencil = { 1.0f, 0 };

		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = vkContext.graphicsPipeline->renderPass.handle.get();
		renderPassInfo.framebuffer = vkContext.swapchainFrameBuffers[vkContext.getCurrentImageIndex()].handle.get();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vkContext.swapchain->extent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		commandBuffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, vkContext.graphicsPipeline->handle.get());
		currentScene.render(*commandBuffer);
		commandBuffer->endRenderPass();

		editorGUI->render(*commandBuffer, vkContext.getCurrentFrameBuffer());
		vkContext.endFrame();
	}
}

bool Renderer::isRunning() const noexcept
{
	return m_isRunning;
}