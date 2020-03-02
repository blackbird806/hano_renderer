#pragma once

#include <glm/glm.hpp>

namespace hano
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;
		int32_t matIndex;
	};
}