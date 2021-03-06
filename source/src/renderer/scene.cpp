#include <renderer/scene.hpp>

using namespace hano;

void Scene::addModel(Model& model)
{
	models.push_back(model);
}

void Scene::addSphere(Sphere const& sphere)
{
	spheres.push_back(sphere);
}

void Scene::addLight(PointLight const& light)
{
	lights.push_back(light);
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

std::vector<Sphere> const& Scene::getSpheres() const
{
	return spheres;
}

std::vector<PointLight> const& Scene::getPointLights() const
{
	return lights;
}
