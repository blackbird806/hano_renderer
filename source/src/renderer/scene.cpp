#include <renderer/scene.hpp>

using namespace hano;

void Scene::render(vk::CommandBuffer commandBuffer)
{
	camera.update();
	for (auto& mesh : meshes)
	{
		mesh.render(commandBuffer, camera);
	}
}

void Scene::handleResizing()
{
	for (auto& mesh : meshes)
	{
		mesh.handleResizing();
	}
}