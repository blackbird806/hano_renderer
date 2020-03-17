#include <glfw/glfw3.h>
#include <imgui/imgui.h>
#include <hanoRenderer.hpp>
#include <core/logger.hpp>

using namespace hano;

void Renderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	app->m_vkContext.frameBufferResized = true;
}

Renderer::Renderer()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);

	glfwSetFramebufferSizeCallback(m_window, &framebufferResizeCallback);
	m_vkContext.init(m_window, { .appName = "hanoRtx", .engineName = "hanoRenderer" });
	m_editorGUI = std::make_unique<EditorGUI>(m_vkContext);
	
	m_vkContext.onRecreateSwapchain = [&]() {
		m_editorGUI->handleSwapchainRecreation();
		m_currentScene->handleResizing();
	};

	float speed = 0;

	m_editorGUI->onGUI = [&, speed]() mutable {
		ImGui::ShowDemoWindow();
		ImGui::ShowMetricsWindow();
		
		ImGui::Begin("my window");
		ImGui::DragFloat("speed", &speed);
		if (ImGui::Button("yikes"))
		{
			m_currentScene->meshes[0].transform.pos.x += speed;
		}

		ImGui::Text("OUI");

		ImGui::End();
	};

	m_isRunning = true;

}

Renderer::~Renderer()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Renderer::setRenderScene(Scene& scene)
{
	m_currentScene = &scene;
}

void Renderer::renderFrame()
{
	// @Review
	m_isRunning = !((bool)glfwWindowShouldClose(m_window));
	glfwPollEvents();
	
	// @Review
	auto commandBuffer = m_vkContext.beginFrame();
	if (commandBuffer)
	{
		std::array<vk::ClearValue, 2> clearValues = {};
		auto e = std::array{ 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[0].color = { e };
		clearValues[1].depthStencil = { 1.0f, 0 };

		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = m_vkContext.graphicsPipeline->renderPass.handle.get();
		renderPassInfo.framebuffer = m_vkContext.swapchainFrameBuffers[m_vkContext.getCurrentImageIndex()].handle.get();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_vkContext.swapchain->extent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		commandBuffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, m_vkContext.graphicsPipeline->handle.get());
		
		/*commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_vkContext.graphicsPipeline->pipelineLayout.get(), 0, 
			{ m_vkContext.graphicsPipeline->descriptorSetManager->descriptorSets->handle(m_vkContext.getCurrentImageIndex()) }, {});*/

		m_currentScene->render(*commandBuffer);
		commandBuffer->endRenderPass();

		m_editorGUI->render(*commandBuffer, m_vkContext.getCurrentFrameBuffer());
		m_vkContext.endFrame();
	}
}

bool Renderer::isRunning() const noexcept
{
	return m_isRunning;
}