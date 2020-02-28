#pragma once

#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>

namespace hano::vkh
{
	struct Device;

	struct ShaderModule
	{
		ShaderModule(Device const& device, std::string const& filename);
		ShaderModule(Device const& device, std::vector<char> const& code);

		vk::PipelineShaderStageCreateInfo createShaderStageInfo(vk::ShaderStageFlagBits stageBits) const;

		vkh::Device const* device;
		vk::UniqueShaderModule handle;
	};
}