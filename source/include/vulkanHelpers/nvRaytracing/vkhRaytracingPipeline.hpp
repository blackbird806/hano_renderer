#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <core/hanoConfig.hpp>
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
		void addHitGroup(std::vector<std::reference_wrapper<vkh::ShaderModule>> shaders, vk::RayTracingShaderGroupTypeNV hitGroupType);
		HANO_NODISCARD RaytracingPipeline create(vkh::DescriptorSetLayout const& descriptorSetLayout_, std::vector<vk::PushConstantRange> pushConstants,uint32_t maxRecursionDepth);

	private:
		std::vector<vk::RayTracingShaderGroupCreateInfoNV> m_shaderGroups;
		std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
	};

}