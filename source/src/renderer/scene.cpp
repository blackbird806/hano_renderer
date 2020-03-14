#include <renderer/scene.hpp>

using namespace hano;

void Scene::render(vk::CommandBuffer commandBuffer) const
{
	for (auto const& mesh : meshes)
	{
		mesh.render(commandBuffer);
	}
}