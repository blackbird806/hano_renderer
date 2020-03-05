#pragma once

#include <memory>
#include <renderer/editorGUI.hpp>
#include "vulkanContext.hpp"

struct GLFWwindow;

namespace hano
{
	class Renderer
	{
		public:

			Renderer();
			~Renderer();
		
			void renderFrame();

			bool isRunning() const noexcept;

		private:

			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

			GLFWwindow* m_window;
			VulkanContext vkContext;
			std::unique_ptr<EditorGUI> editorGUI;
			bool m_isRunning;
	};
}
