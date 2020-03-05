#pragma once

#include <glm/glm.hpp>
#include <vulkanHelpers/vkhBuffer.hpp>

namespace hano
{
	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};
}