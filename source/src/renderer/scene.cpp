#include <renderer/scene.hpp>

using namespace hano;

void Scene::addModel(Model& model)
{
	models.push_back(model);
}

void Scene::render(vk::CommandBuffer commandBuffer)
{
	camera.updateViewMtr();
	for (Model& model : models)
	{
		model.render(commandBuffer);
	}
}

std::vector<std::reference_wrapper<Model>> const& Scene::getModels() const
{
	return models;
}