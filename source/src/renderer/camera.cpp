#include <renderer/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace hano;

void Camera::setPerspectiveProjection(float fov, glm::vec2 view, float near, float far) noexcept
{
	projectionMtr = glm::perspective(glm::radians(fov), view.x/view.y, near, far);
	// Vulkan clip space has inverted Y and half Z.
	glm::mat4 const clip(	1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, -1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.5f, 0.0f,
							0.0f, 0.0f, 0.5f, 1.0f);
	//projectionMtr = clip * projectionMtr; 
}

void Camera::setOrthographicProjection(glm::vec2 view) noexcept
{
	projectionMtr = glm::ortho(0.0f, view.x, 0.0f, view.y);
}

void Camera::updateViewMtr() noexcept
{
	viewMtr = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}