#pragma once

#include <memory>
#include <renderer/editorGUI.hpp>
#include <renderer/scene.hpp>
#include "vulkanContext.hpp"

struct GLFWwindow;

namespace hano
{
	class Renderer
	{
		public:

			Renderer();
			~Renderer();

			void setRenderScene(Scene const& scene);
			void renderFrame();

			bool isRunning() const noexcept;

		private:

			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

			GLFWwindow* m_window;
			VulkanContext m_vkContext;
			std::unique_ptr<EditorGUI> m_editorGUI;
			Scene const* m_currentScene;
			bool m_isRunning;
	};
}
