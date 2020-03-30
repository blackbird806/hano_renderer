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
		void setPerspectiveProjection(float fov, glm::vec2 view, float near, float far) noexcept;
		void setOrthographicProjection(glm::vec2 view) noexcept;

		// @Review
		void update() noexcept;

		glm::vec3 pos;
		glm::quat rot;
		glm::mat4 projectionMtr;
		glm::mat4 viewMtr;
	};
}