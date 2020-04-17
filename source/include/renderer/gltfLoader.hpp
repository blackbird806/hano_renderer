#pragma once

#include <filesystem>
#include <hanoRenderer.hpp>
#include "model.hpp"

namespace hano
{
	Model loadGltfModel(Renderer& renderer, std::filesystem::path const& gltfPath);
}