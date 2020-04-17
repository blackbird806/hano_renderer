#pragma once
#include "texture.hpp"

namespace hano
{
	class Material
	{
		public:

			float roughnessFactor;
			float metallicFactor;
			float baseColorFactor;

			Texture* baseColor;
			Texture* normalMap;
			Texture* metallicRoughness;
			Texture* emissive;
			Texture* occlusion;
	};
}