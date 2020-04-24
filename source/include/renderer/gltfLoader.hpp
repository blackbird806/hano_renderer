#pragma once

#include <filesystem>
#include <hanoRenderer.hpp>
#include "model.hpp"

namespace hano
{
	struct GltfLoadedResources
	{
		Mesh mesh;
		Material material;
		std::vector<Texture> textures;
	};

	GltfLoadedResources loadGltfModel(Renderer& renderer, std::filesystem::path const& gltfPath);
}