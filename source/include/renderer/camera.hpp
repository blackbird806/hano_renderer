#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace hano
{
	struct Camera
	{
		void setPerspectiveProjection() noexcept;
		void setOrthographicProjection() noexcept;

		float near, far;
		glm::vec2 view;
		float fov;

		glm::vec3 pos;
		glm::quat rot;
		glm::mat4 projectionMtr;
		glm::mat4 viewMtr;
	};
}