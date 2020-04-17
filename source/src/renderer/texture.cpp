#include <renderer/texture.hpp>

#include <stb/stb_image.h>
#include <vulkanHelpers/vkhBuffer.hpp>
#include "vulkanContext.hpp"

using namespace hano;

Texture::Texture(VulkanContext const& ctx, std::filesystem::path const& texturePath)
    : vkContext(&ctx)
{
	load(texturePath);
}

void Texture::load(std::filesystem::path const& texturePath)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(texturePath.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha); // @TODO alpha
    vk::DeviceSize imageSize = texWidth * texHeight * STBI_rgb_alpha;

    if (!pixels) {
        throw HanoException("failed to load texture image!");
    }

    vkh::Buffer stagingBuffer(*vkContext->device, imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    
    void* data = stagingBuffer.map();
        memcpy(data, pixels, static_cast<size_t>(imageSize));
    stagingBuffer.unMap();
    stbi_image_free(pixels);
	
	image.init(*vkContext->device, vk::Extent2D(texWidth, texHeight), vk::Format::eR8G8B8A8Srgb, vk::MemoryPropertyFlagBits::eDeviceLocal);
	image.transitionImageLayout(*vkContext->commandPool, vk::ImageLayout::eTransferDstOptimal);
	image.copyFrom(*vkContext->commandPool, stagingBuffer);
	image.transitionImageLayout(*vkContext->commandPool, vk::ImageLayout::eShaderReadOnlyOptimal);

	imageView.init(*vkContext->device, image.handle.get(), vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
	
	vk::SamplerCreateInfo samplerInfo;
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.minFilter = vk::Filter::eLinear;
	samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16.0f;
	samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = vk::CompareOp::eAlways;
	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	sampler = vkContext->device->handle.createSamplerUnique(samplerInfo, vkContext->device->allocator);
}

void Texture::init(VulkanContext const& ctx, std::filesystem::path const& texturePath)
{
	vkContext = &ctx;
	load(texturePath);
}

void Texture::destroy()
{
	imageView.destroy();
	image.destroy();
	sampler.reset();
}