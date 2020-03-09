#include <vulkanHelpers/nvRaytracing/vkhRaytracingPipeline.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <vulkanHelpers/vkhDescriptorSetLayout.hpp>

using namespace hano::vkh;

void RaytracingPipelineGenerator::addHitGroup(std::vector<vkh::ShaderModule*> shaders, vk::RayTracingShaderGroupTypeNV hitGroupType)
{
	vk::RayTracingShaderGroupCreateInfoNV groupInfo;
	groupInfo.type = hitGroupType;

	m_shaderStages.reserve(shaders.size());
	for (int index = 0; auto const& shaderPtr : shaders)
	{
		assert(shaderPtr);
		auto& shader = *shaderPtr;
		m_shaderStages.push_back(shader.createShaderStageInfo());
		switch (shader.shaderStage)
		{
		case vk::ShaderStageFlagBits::eClosestHitNV:
			assert(hitGroupType == vk::RayTracingShaderGroupTypeNV::eTrianglesHitGroup);
			groupInfo.closestHitShader = index;
			break;

		case vk::ShaderStageFlagBits::eAnyHitNV:
			assert(hitGroupType == vk::RayTracingShaderGroupTypeNV::eTrianglesHitGroup);
			groupInfo.anyHitShader = index;
			break;

		case vk::ShaderStageFlagBits::eIntersectionNV:
			assert(hitGroupType == vk::RayTracingShaderGroupTypeNV::eProceduralHitGroup);
			groupInfo.intersectionShader = index;
			break;

		case vk::ShaderStageFlagBits::eMissNV:
			assert(hitGroupType == vk::RayTracingShaderGroupTypeNV::eGeneral);
			break;

		case vk::ShaderStageFlagBits::eRaygenNV:
			assert(hitGroupType == vk::RayTracingShaderGroupTypeNV::eGeneral);
			break;

		default:
			assert(false); // bad shader type 
			break;
		}
		index++;
	}

	m_shaderGroups.push_back(groupInfo);
}

RaytracingPipeline RaytracingPipelineGenerator::create(vkh::DescriptorSetLayout descriptorSetLayout_, uint32_t maxRecursionDepth)
{
	RaytracingPipeline pipeline;

	vk::PipelineLayoutCreateInfo layoutInfo;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &descriptorSetLayout_.handle.get();
	layoutInfo.pushConstantRangeCount = 0;
	layoutInfo.pPushConstantRanges = 0;

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

