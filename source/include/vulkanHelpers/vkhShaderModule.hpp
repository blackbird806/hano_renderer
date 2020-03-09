#pragma once

#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>

namespace hano::vkh
{
	struct Device;

	struct ShaderModule
	{
		ShaderModule(Device const& device, std::string const& filename, vk::ShaderStageFlagBits shaderStage_);
		ShaderModule(Device const& device, std::vector<char> const& code, vk::ShaderStageFlagBits shaderStage_);

		vk::PipelineShaderStageCreateInfo createShaderStageInfo() const;

		vkh::Device const* device;
		vk::ShaderStageFlagBits shaderStage;
		vk::UniqueShaderModule handle;
	};
}