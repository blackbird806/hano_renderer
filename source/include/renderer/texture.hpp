#pragma once

#include <filesystem>
#include <vulkanHelpers/vkhImage.hpp>
#include <vulkanHelpers/vkhImageView.hpp>

namespace hano
{
	struct VulkanContext;

	class Texture
	{

		public:
			Texture() = default;
			Texture(VulkanContext const& ctx, std::filesystem::path const& texturePath);
			void init(VulkanContext const& ctx, std::filesystem::path const& texturePath);
			void destroy();

			Texture(Texture&&) = default;
			Texture& operator=(Texture&&) = default;


			// @TOOD
		//private:
			void load(std::filesystem::path const& texturePath);

			VulkanContext const* vkContext;
			vkh::Image image;
			vkh::ImageView imageView;
			vk::UniqueSampler sampler;
	};
}