#include <vulkanHelpers/nvRaytracing/vkhRaytracingPipeline.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhDescriptorSetLayout.hpp>

using namespace hano::vkh;

// @TO REDO
void RaytracingPipelineGenerator::addHitGroup(std::vector<std::reference_wrapper<vkh::ShaderModule>> shaders, vk::RayTracingShaderGroupTypeNV hitGroupType)
{
	vk::RayTracingShaderGroupCreateInfoNV groupInfo;
	groupInfo.type = hitGroupType;
	groupInfo.generalShader = VK_SHADER_UNUSED_NV;
	groupInfo.closestHitShader = VK_SHADER_UNUSED_NV;
	groupInfo.intersectionShader = VK_SHADER_UNUSED_NV;
	groupInfo.anyHitShader = VK_SHADER_UNUSED_NV;
	
	m_shaderStages.reserve(m_shaderStages.size() + shaders.size());
	for (auto const& shaderRef : shaders)
	{
		auto& shader = shaderRef.get();
		uint32 index = m_shaderStages.size();
		m_shaderStages.push_back(shader.createShaderStageInfo());
		switch (shader.shaderStage)
		{
		case vk::ShaderStageFlagBits::eClosestHitNV:
			assert(	hitGroupType == vk::RayTracingShaderGroupTypeNV::eTrianglesHitGroup || 
					hitGroupType == vk::RayTracingShaderGroupTypeNV::eProceduralHitGroup);
			groupInfo.closestHitShader = index;
			break;

		case vk::ShaderStageFlagBits::eAnyHitNV:
			assert(	hitGroupType == vk::RayTracingShaderGroupTypeNV::eTrianglesHitGroup ||
					hitGroupType == vk::RayTracingShaderGroupTypeNV::eProceduralHitGroup);
			groupInfo.anyHitShader = index;
			break;

		case vk::ShaderStageFlagBits::eIntersectionNV:
			assert(hitGroupType == vk::RayTracingShaderGroupTypeNV::eProceduralHitGroup);
			groupInfo.intersectionShader = index;
			break;

		case vk::ShaderStageFlagBits::eMissNV:
			assert(hitGroupType == vk::RayTracingShaderGroupTypeNV::eGeneral);
			assert(shaders.size() == 1); // general groups have only one shader
			groupInfo.generalShader = index;
			break;

		case vk::ShaderStageFlagBits::eRaygenNV:
			assert(hitGroupType == vk::RayTracingShaderGroupTypeNV::eGeneral);
			assert(shaders.size() == 1);
			groupInfo.generalShader = index;
			break;

		default:
			assert(false); // bad shader type 
			break;
		}
		index++;
	}

	m_shaderGroups.push_back(groupInfo);
}

RaytracingPipeline RaytracingPipelineGenerator::create(vkh::DescriptorSetLayout const& descriptorSetLayout_, std::vector<vk::PushConstantRange> pushConstants, uint32_t maxRecursionDepth)
{
	RaytracingPipeline pipeline;

	vk::PipelineLayoutCreateInfo layoutInfo;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &descriptorSetLayout_.handle.get();
	layoutInfo.pushConstantRangeCount = std::size(pushConstants);
	layoutInfo.pPushConstantRanges = pushConstants.data();

	pipeline.pipelineLayout = descriptorSetLayout_.device->handle.createPipelineLayoutUnique(layoutInfo, descriptorSetLayout_.device->allocator);

	vk::RayTracingPipelineCreateInfoNV pipelineInfo;
	pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
	pipelineInfo.pStages = m_shaderStages.data();
	pipelineInfo.groupCount = static_cast<uint32_t>(m_shaderGroups.size());
	pipelineInfo.pGroups = m_shaderGroups.data();
	pipelineInfo.maxRecursionDepth = maxRecursionDepth;
	pipelineInfo.layout = pipeline.pipelineLayout.get();
	pipelineInfo.basePipelineHandle = nullptr;
	pipelineInfo.basePipelineIndex = 0;

	pipeline.handle = descriptorSetLayout_.device->handle.createRayTracingPipelineNVUnique(nullptr, pipelineInfo, descriptorSetLayout_.device->allocator);
	
	return pipeline;
}

