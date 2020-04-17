#pragma once

#include <filesystem>
#include <vulkanHelpers/vkhImage.hpp>
#include <vulkanHelpers/vkhImageView.hpp>
#include <span>

namespace hano
{
	struct VulkanContext;

	class Texture
	{

		public:

			static vk::SamplerCreateInfo defaultSampler();

			Texture() = default;
			Texture(VulkanContext const& ctx, std::filesystem::path const& texturePath);
			void init(VulkanContext const& ctx, std::filesystem::path const& texturePath);
			void init(VulkanContext const& ctx, std::span<byte> imageBuffer);
			void destroy();

			Texture(Texture&&) = default;
			Texture& operator=(Texture&&) = default;


			// @TODO
		//private:
			void load(std::filesystem::path const& texturePath);
			void createGPUResources(std::span<byte> imageBuffer, vk::SamplerCreateInfo samplerInfo, vk::Format imageFormat,int width, int height, int channels);

			VulkanContext const* vkContext;
			vkh::Image image;
			vkh::ImageView imageView;
			vk::UniqueSampler sampler;
	};
}