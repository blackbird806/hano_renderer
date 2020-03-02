#pragma once

#include <memory>
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
	};
}
