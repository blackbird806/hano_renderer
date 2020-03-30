#include <glfw/glfw3.h>
#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <hanoRenderer.hpp>
#include <core/logger.hpp>

using namespace hano;

void Renderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	app->m_vkContext.frameBufferResized = true;
	app->m_windowWidth = width;
	app->m_windowHeight = height;
}

Renderer::Renderer() 
	: m_window(nullptr), m_windowWidth(0), m_windowHeight(0), m_currentScene(nullptr), m_editorGUI(nullptr), m_isRunning(false)
{

}

Renderer::Renderer(Renderer::Info const& infos)
{
	init(infos);
}

void Renderer::init(Renderer::Info const& infos)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(infos.windowWidth, infos.windowHeight, infos.appName, nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);

	m_windowWidth = infos.windowWidth;
	m_windowHeight = infos.windowHeight;

	glfwSetFramebufferSizeCallback(m_window, &framebufferResizeCallback);
	m_vkContext.init(m_window, { .appName = infos.appName, .engineName = c_engineName });

	m_editorGUI = std::make_unique<EditorGUI>(m_vkContext);

	// @Review
	m_vkContext.onRecreateSwapchain = [&]() {
		m_editorGUI->handleSwapchainRecreation();
	};
	
	m_resourceMgr.init(m_vkContext);

	m_isRunning = true;
}

void Renderer::waitIdle() const
{
	assert(m_vkContext.device);
	m_vkContext.device->handle.waitIdle();
}

void Renderer::destroy()
{
	if (m_vkContext.device)
		m_vkContext.device->handle.waitIdle();

	m_resourceMgr.releaseResources();
	m_editorGUI.reset();
	m_vkContext.destroy();

	if (m_window)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		m_window = nullptr;
	}
}

Renderer::~Renderer()
{
	destroy();
}

Mesh& hano::Renderer::loadMesh(std::filesystem::path const& meshPath)
{
	return m_resourceMgr.loadMesh(meshPath);
}

Texture& Renderer::loadTexture(std::filesystem::path const& texturePath)
{
	return m_resourceMgr.loadTexture(texturePath);
}

void Renderer::setRenderScene(Scene& scene)
{
	m_currentScene = &scene;

	m_vkContext.createRaytracingOutImage();
	m_vkContext.createRtStructures(scene);
	m_vkContext.createRaytracingPipeline();
	m_vkContext.createShaderBindingTable();
}

void Renderer::setEditorGUI(CustomEditorGUI& editor)
{
	m_editorGUI->setCustomEditor(&editor);
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
		//std::array<vk::ClearValue, 2> clearValues = {};
		//auto e = std::array{ 0.0f, 0.0f, 0.0f, 1.0f };
		//clearValues[0].color = { e };
		//clearValues[1].depthStencil = { 1.0f, 0 };

		//vk::RenderPassBeginInfo renderPassInfo = {};
		//renderPassInfo.renderPass = m_vkContext.graphicsPipeline->renderPass.handle.get();
		//renderPassInfo.framebuffer = m_vkContext.swapchainFrameBuffers[m_vkContext.getCurrentImageIndex()].handle.get();
		//renderPassInfo.renderArea.offset = { 0, 0 };
		//renderPassInfo.renderArea.extent = m_vkContext.swapchain->extent;
		//renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		//renderPassInfo.pClearValues = clearValues.data();

		//commandBuffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		//commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, m_vkContext.graphicsPipeline->handle.get());
		//
		//m_currentScene->render(*commandBuffer);
		//commandBuffer->endRenderPass();
		m_vkContext.raytrace(*commandBuffer);
		m_editorGUI->render(*commandBuffer, m_vkContext.getCurrentFrameBuffer());
		m_vkContext.endFrame();
	}
}

bool Renderer::isRunning() const noexcept
{
	return m_isRunning;
}

int Renderer::getWindowWidth() const noexcept
{
	return m_windowWidth;
}

int Renderer::getWindowHeight() const noexcept
{
	return m_windowHeight;
}

Scene* Renderer::getCurrentScene()
{
	return m_currentScene;
}
