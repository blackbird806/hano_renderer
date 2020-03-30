#include <renderer/resourceManager.hpp>

using namespace hano;

void ResourceManager::init(VulkanContext& ctx)
{
	m_vkContext = &ctx;
}

Mesh& ResourceManager::loadMesh(std::filesystem::path const& meshPath)
{
	auto [it, _] = m_meshes.try_emplace(meshPath.filename().string(), Mesh(*m_vkContext, meshPath));
	return it->second;
}

Texture& ResourceManager::loadTexture(std::filesystem::path const& texturePath)
{
	auto [it, _] = m_textures.try_emplace(texturePath.filename().string(), Texture(*m_vkContext, texturePath));
	return it->second;
}

void ResourceManager::releaseResources()
{
	m_meshes.clear();
	m_textures.clear();
}
