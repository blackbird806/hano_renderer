#pragma once
#include <vector>
#include "texture.hpp"

namespace hano
{
	class Material
	{
		public:


		private:
			
			vk::Pipeline m_pipeline;
			vk::PipelineBindPoint m_bindPoint;
			std::vector<Texture> m_textures;
	};
}