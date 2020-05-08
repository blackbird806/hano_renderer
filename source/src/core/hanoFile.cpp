#include <fstream>
#include <vector>
#include <core/hanoFile.hpp>
#include <core/logger.hpp>
#include <core/hanoException.hpp>

std::vector<char> hano::readFile(std::filesystem::path const& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw hano::HanoException(fmt::format("failed to open file : {}", filePath.string()));

	const auto fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}
