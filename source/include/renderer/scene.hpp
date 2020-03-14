#pragma once

#include <vector>
#include <vulkanHelpers/vkhBuffer.hpp>
#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"

namespace hano
{
	class Scene
	{
		public:

			void render(vk::CommandBuffer commandBuffer) const;

			Camera camera;
			std::vector<Mesh> meshes;
			std::vector<vkh::Buffer> uniformBuffers;
			std::vector<PointLight> lights;
	};
}