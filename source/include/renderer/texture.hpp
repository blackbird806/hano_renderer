#pragma once

#include <filesystem>
#include <vulkanHelpers/vkhImage.hpp>
#include <vulkanHelpers/vkhImageView.hpp>
#include "vulkanContext.hpp"

namespace hano
{
	class Texture
	{

		public:
			Texture() = default;
			Texture(VulkanContext const& ctx, std::filesystem::path const& texturePath);
			Texture(Texture&&) = default;
			Texture& operator=(Texture&&) = default;

			void load(std::filesystem::path const& texturePath);

		private:

			VulkanContext const* vkContext;
			vkh::Image image;
			vkh::ImageView imageView;
			vk::UniqueSampler sampler;
	};
}