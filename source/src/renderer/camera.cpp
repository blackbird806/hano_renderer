#include <renderer/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace hano;

void Camera::setPerspectiveProjection() noexcept
{
	projectionMtr = glm::perspective(fov, view.x/view.y, near, far);
}

void Camera::setOrthographicProjection() noexcept
{
	assert(false);
}
