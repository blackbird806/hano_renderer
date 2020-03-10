#pragma once

#include <vector>
#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"

namespace hano
{
	class Scene
	{
		public:

			void render(vk::CommandBuffer commandBuffer);

			Camera camera;
			std::vector<Mesh> meshes;
			std::vector<PointLight> lights;
	};
}