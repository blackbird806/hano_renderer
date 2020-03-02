#pragma once

#include <memory>
#include <renderer/editorGUI.hpp>
#include "vulkanContext.hpp"

namespace hano
{
	class Renderer
	{
		public:

			Renderer();
			~Renderer();
		
		private:

			struct GLFWwindow* m_window;
			VulkanContext vkContext;
			std::unique_ptr<EditorGUI> editorGUI;
	};
}
