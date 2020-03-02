#include <glfw/glfw3.h>
#include <hanoRenderer.hpp>
#include <core/logger.hpp>

using namespace hano;

Renderer::Renderer()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
	vkContext.init(m_window, { .appName = "hanoRtx", .engineName = "hanoRenderer" });
	editorGUI = std::make_unique<EditorGUI>(vkContext);
}

Renderer::~Renderer()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}
