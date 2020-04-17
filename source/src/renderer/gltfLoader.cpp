#include <renderer/gltfLoader.hpp>
#include <tiny/tiny_gltf.h>

#include <renderer/texture.hpp>

namespace
{
	vk::Filter to_vkFilter(int gltfFilter)
	{
		switch (gltfFilter)
		{
			case TINYGLTF_TEXTURE_FILTER_NEAREST:
			case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
			case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
				return vk::Filter::eNearest;
				break;
			case TINYGLTF_TEXTURE_FILTER_LINEAR:
			case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
			case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
				return vk::Filter::eLinear;
		}
	}

	vk::SamplerAddressMode to_vkSamplerAdressMode(int gltfwrapMode)
	{
		switch (gltfwrapMode)
		{
		case TINYGLTF_TEXTURE_WRAP_REPEAT:
			return vk::SamplerAddressMode::eRepeat;
		case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
			return vk::SamplerAddressMode::eClampToEdge;
		case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
			return vk::SamplerAddressMode::eMirroredRepeat;
		}
	}

	std::vector<hano::Texture> loadTextures(tinygltf::Model const& gltfModel, hano::Renderer& renderer)
	{
		// load samplers
		std::vector<vk::SamplerCreateInfo> samplers;
		samplers.reserve(gltfModel.samplers.size());

		for (auto const& gltfSampler : gltfModel.samplers)
		{
			vk::SamplerCreateInfo vkSamplerInfo;
			vkSamplerInfo.minFilter = to_vkFilter(gltfSampler.minFilter);
			vkSamplerInfo.magFilter = to_vkFilter(gltfSampler.magFilter);
			vkSamplerInfo.addressModeU = to_vkSamplerAdressMode(gltfSampler.wrapS);
			vkSamplerInfo.addressModeV = to_vkSamplerAdressMode(gltfSampler.wrapT);
			vkSamplerInfo.addressModeW = to_vkSamplerAdressMode(gltfSampler.wrapR);
			samplers.push_back(vkSamplerInfo);
		}

		//load textures
		std::vector<hano::Texture> textures;
		textures.reserve(gltfModel.textures.size());

		for (auto const& gltfTexture : gltfModel.textures) {
			tinygltf::Image image = gltfModel.images[gltfTexture.source];
			vk::SamplerCreateInfo samplerInfo;
			if (gltfTexture.sampler == -1) {
				// No sampler specified, use a default one
				samplerInfo.magFilter = vk::Filter::eLinear;
				samplerInfo.minFilter = vk::Filter::eLinear;
				samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
				samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
				samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
			}
			else {
				samplerInfo = samplers[gltfTexture.sampler];
			}
			
			tinygltf::Image img = gltfModel.images[gltfTexture.source];

			//hano::Texture texture;

			//textures.push_back(texture);
		}
		return textures;
	}
}

// @Review
hano::Model hano::loadGltfModel(hano::Renderer& renderer, std::filesystem::path const& gltfPath)
{
	using namespace tinygltf;

	tinygltf::Model model;
	TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, gltfPath.string());
	//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

	if (!warn.empty()) {
		hano_warnf("Warn: %s\n", warn.c_str());
	}

	if (!err.empty()) {
		hano_errorf("Err: %s\n", err.c_str());
	}

	if (!ret) {
		throw HanoException("Failed to parse glTF\n");
	}

	Mesh mesh(renderer.getVkContext());
	mesh.loadGltf(model);


}