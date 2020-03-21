#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace hano
{
	struct Transform
	{
		glm::vec3 pos;
		glm::quat rot;
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		glm::mat4 getMatrix() const noexcept
		{
			return glm::translate(glm::mat4(), pos)* glm::scale(glm::mat4(), scale)* glm::toMat4(rot);
		}
	};
}