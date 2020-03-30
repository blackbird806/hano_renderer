#pragma once

#include <vector>
#include <functional>
#include <vulkanHelpers/vkhBuffer.hpp>
#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"

namespace hano
{
	class Scene
	{
		public:

			void addModel(Model& model);
			void render(vk::CommandBuffer commandBuffer);

			// @Review
			Camera camera;

		private:

			std::vector<std::reference_wrapper<Model>> models;
			std::vector<PointLight> lights;
	};
}