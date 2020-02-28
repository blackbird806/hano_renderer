#pragma once

#include <vulkan/vulkan.hpp>

namespace hano::vkh
{
	struct DescriptorBinding
	{
		uint32_t binding; // Slot to which the descriptor will be bound, corresponding to the layout index in the shader.
		uint32_t descriptorCount; // Number of descriptors to bind
		vk::DescriptorType type; // Type of the bound descriptor(s)
		vk::ShaderStageFlags stage; // Shader stage at which the bound resources will be available
	};
}