#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkanHelpers/vkhShaderModule.hpp>

namespace hano::vkh
{
	struct Device;
	struct DescriptorSetLayout;

	struct RaytracingPipeline
	{
		vk::UniquePipeline handle;
		vk::UniquePipelineLayout pipelineLayout;
	};

	// helper struct used to create RaytracingPipeline
	struct RaytracingPipelineGenerator
	{
		void addHitGroup(std::vector<vkh::ShaderModule*> shaders, vk::RayTracingShaderGroupTypeNV hitGroupType);
		RaytracingPipeline create(vkh::DescriptorSetLayout descriptorSetLayout_, uint32_t maxRecursionDepth);

		std::vector<vk::RayTracingShaderGroupCreateInfoNV> m_shaderGroups;
		std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
	};

}