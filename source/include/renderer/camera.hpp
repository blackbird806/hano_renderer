#pragma once
#define WIN32_LEAN_AND_MEAN
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

// windows.h <3
#undef near
#undef far

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