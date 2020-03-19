#pragma once
#define WIN32_LEAN_AND_MEAN
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

// windows.h <3
#undef near
#undef far

namespace hano
{
	// used for UBO
	struct CameraMatrices
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct Camera
	{
		void setPerspectiveProjection() noexcept;
		void setOrthographicProjection() noexcept;

		void update() noexcept;

		float near = 0.01f, far = 10000.0f;
		glm::vec2 view;
		float fov = 45.0f;

		glm::vec3 pos;
		glm::quat rot;
		glm::mat4 projectionMtr;
		glm::mat4 viewMtr;
	};
}