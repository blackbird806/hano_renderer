#include <renderer/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace hano;

void Camera::setPerspectiveProjection() noexcept
{
	projectionMtr = glm::perspective(glm::radians(fov), view.x/view.y, near, far);
}

void Camera::setOrthographicProjection() noexcept
{
	assert(false);
}

void Camera::update() noexcept
{
	viewMtr = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}