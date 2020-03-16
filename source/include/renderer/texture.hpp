#pragma once

#include <filesystem>
#include <vulkanHelpers/vkhImage.hpp>
#include <vulkanHelpers/vkhImageView.hpp>

namespace hano
{
	class Texture
	{
		Texture();
		Texture(std::filesystem::path texturePath);
		Texture(Texture&&);

		Texture& operator=(Texture&&);

		void load(std::filesystem::path texturePath);

		private:

			vkh::ImageView imageView;
	};
}