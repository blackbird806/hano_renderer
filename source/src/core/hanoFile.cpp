#include <fstream>
#include <vector>
#include <core/hanoFile.hpp>
#include <core/logger.hpp>

std::vector<char> hano::readFile(std::filesystem::path const& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		error("failed to open file : ", filePath);
		// @Review maybe throw here
		return {};
	}

	const auto fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}
