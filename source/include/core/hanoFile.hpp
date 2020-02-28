#pragma once

#include <filesystem>
#include <vector>
#include "hanoConfig.hpp"

namespace hano
{
	HANO_NODISCARD std::vector<char> readFile(std::filesystem::path const& filePath);
}