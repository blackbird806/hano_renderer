#pragma once
#include <glm/glm.hpp>

namespace hano
{
	struct PointLight
	{
		float intensity;
		glm::vec3 pos;

		// @Review maybe create a Color class ?
		glm::vec3 color;
	};
}