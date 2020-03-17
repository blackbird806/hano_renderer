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

			void setRenderScene(Scene& scene);
			void renderFrame();

			bool isRunning() const noexcept;

			// @TMP
			VulkanContext m_vkContext;

		private:

			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

			GLFWwindow* m_window;
			std::unique_ptr<EditorGUI> m_editorGUI;
			Scene* m_currentScene;
			bool m_isRunning;
	};
}
