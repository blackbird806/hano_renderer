#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace hano
{
	struct Transform
	{
		glm::vec3 pos;
		glm::quat rot;
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	};
}