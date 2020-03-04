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
		editorGUI.reset();
		editorGUI = std::make_unique<EditorGUI>(vkContext);
	};
}

Renderer::~Renderer()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Renderer::renderFrame()
{
	glfwPollEvents();
	auto commandBuffer = vkContext.beginFrame();
	if (commandBuffer)
	{
		editorGUI->render(*commandBuffer, vkContext.getCurrentFrameBuffer());
		vkContext.endFrame();
	}
}