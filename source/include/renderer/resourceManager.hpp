#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include "vulkanContext.hpp"
#include "mesh.hpp"
#include "texture.hpp"

namespace hano
{
	class ResourceManager
	{

	public:

		void init(VulkanContext& ctx);

		Mesh& loadMesh(std::filesystem::path const& meshPath);
		Texture& loadTexture(std::filesystem::path const& texturePath);

		void releaseResources();

	private:

		VulkanContext* m_vkContext;
		std::unordered_map<std::string, Mesh> m_meshes;
		std::unordered_map<std::string, Texture> m_textures;
	};

}