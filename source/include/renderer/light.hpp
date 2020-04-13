#pragma once
#include <glm/glm.hpp>

namespace hano
{
	struct PointLight
	{
		alignas(8)  float intensity;
		alignas(16) glm::vec3 pos;
		alignas(16) glm::vec3 color;
	};
}