#pragma once
#include <glm/glm.hpp>

namespace hano
{
	struct Sphere
	{
		glm::vec3 pos;
		float radius;
	};

	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
	};
}