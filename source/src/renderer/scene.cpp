#include <renderer/scene.hpp>

using namespace hano;

void Scene::render(vk::CommandBuffer commandBuffer)
{
	for (auto& mesh : meshes)
	{
		mesh.render(commandBuffer);
	}
}

void Scene::handleResizing()
{
	for (auto& mesh : meshes)
	{
		mesh.handleResizing();
	}
}