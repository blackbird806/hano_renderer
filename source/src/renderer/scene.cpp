#include <renderer/scene.hpp>

using namespace hano;

void Scene::addModel(Model& model)
{
	models.push_back(model);
}

void Scene::render(vk::CommandBuffer commandBuffer)
{
	camera.update();
	for (Model& model : models)
	{
		model.render(commandBuffer);
	}
}
