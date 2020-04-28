#pragma once

#include <vector>
#include <functional>
#include <vulkanHelpers/vkhBuffer.hpp>
#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "shapes.hpp"

namespace hano
{
	class Scene
	{
		public:

			static constexpr uint c_maxLights = 4;

			void addModel(Model& model);
			void addSphere(Sphere const& sphere);
			void addLight(PointLight const& light);
			void render(vk::CommandBuffer commandBuffer);

			// @Review
			std::vector<std::reference_wrapper<Model>> const& getModels() const;
			std::vector<Sphere> const& getSpheres() const;
			std::vector<PointLight> const& getPointLights() const;

			// @Review
			Camera camera;
			std::vector<PointLight> lights;
		private:
			std::vector<Sphere> spheres;
			std::vector<std::reference_wrapper<Model>> models;
	};
}