#include <renderer/texture.hpp>

#include <stb/stb_image.h>
#include <vulkanHelpers/vkhBuffer.hpp>
#include "vulkanContext.hpp"

using namespace hano;

vk::SamplerCreateInfo Texture::defaultSampler()
{
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
	return samplerInfo;
}


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

	createGPUResources(std::span(pixels, imageSize), defaultSampler(), vk::Format::eR8G8B8A8Srgb, texWidth, texHeight, texChannels);
    stbi_image_free(pixels);
}

void Texture::createGPUResources(std::span<byte> imageBuffer, vk::SamplerCreateInfo samplerInfo, vk::Format imageFormat, int width, int height, int channels)
{
	vkh::Buffer stagingBuffer(*vkContext->device, imageBuffer.size() * sizeof(byte), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	stagingBuffer.setDataArray(imageBuffer);

	image.init(*vkContext->device, vk::Extent2D(width, height), imageFormat, vk::MemoryPropertyFlagBits::eDeviceLocal);
	image.transitionImageLayout(*vkContext->commandPool, vk::ImageLayout::eTransferDstOptimal);
	image.copyFrom(*vkContext->commandPool, stagingBuffer);
	image.transitionImageLayout(*vkContext->commandPool, vk::ImageLayout::eShaderReadOnlyOptimal);

	imageView.init(*vkContext->device, image.handle.get(), imageFormat, vk::ImageAspectFlagBits::eColor);

	sampler = vkContext->device->handle.createSamplerUnique(samplerInfo, vkContext->device->allocator);
}

void Texture::init(VulkanContext const& ctx, std::filesystem::path const& texturePath)
{
	vkContext = &ctx;
	load(texturePath);
}

void Texture::init(VulkanContext const& ctx, std::span<byte> imageBuffer, vk::SamplerCreateInfo samplerInfo, vk::Format imageFormat, int width, int height, int channels)
{
	vkContext = &ctx;
	createGPUResources(imageBuffer, samplerInfo, imageFormat, width, height, channels);
}

void Texture::destroy()
{
	imageView.destroy();
	image.destroy();
	sampler.reset();
}