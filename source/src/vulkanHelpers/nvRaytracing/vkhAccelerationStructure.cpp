#include <vulkanHelpers/nvRaytracing/vkhAccelerationStructure.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

AccelerationStructure::AccelerationStructure(vkh::Device const& device_, vk::AccelerationStructureCreateInfoNV const& createInfo)
	: device(&device_), allowUpdate(createInfo.info.flags & vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate)
{
	handle = device->handle.createAccelerationStructureNVUnique(createInfo, device->allocator);
}

AccelerationStructure::MemoryRequirements AccelerationStructure::getMemoryRequirements() const
{
	vk::AccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo{};
	memoryRequirementsInfo.pNext = nullptr;
	memoryRequirementsInfo.accelerationStructure = handle.get();

	auto& deviceHandle = device->handle;

	vk::MemoryRequirements2 memoryRequirements = {};
	memoryRequirementsInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eObject;
	memoryRequirements = deviceHandle.getAccelerationStructureMemoryRequirementsNV(memoryRequirementsInfo);
	auto const resultRequirements = memoryRequirements.memoryRequirements;

	memoryRequirementsInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eBuildScratch;
	memoryRequirements = deviceHandle.getAccelerationStructureMemoryRequirementsNV(memoryRequirementsInfo);
	auto const buildRequirements = memoryRequirements.memoryRequirements;

	memoryRequirementsInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eUpdateScratch;
	memoryRequirements = deviceHandle.getAccelerationStructureMemoryRequirementsNV(memoryRequirementsInfo);

	auto const updateRequirements = memoryRequirements.memoryRequirements;

	return { resultRequirements, buildRequirements, updateRequirements };
}

void AccelerationStructure::memoryBarrier(vk::CommandBuffer commandBuffer)
{
	// Wait for the builder to complete by setting a barrier on the resulting buffer. This is
	// particularly important as the construction of the top-level hierarchy may be called right
	// afterwards, before executing the command list.
	vk::MemoryBarrier memoryBarrier = {};
	memoryBarrier.pNext = nullptr;
	memoryBarrier.srcAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteNV | vk::AccessFlagBits::eAccelerationStructureReadNV;
	memoryBarrier.dstAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteNV | vk::AccessFlagBits::eAccelerationStructureReadNV;

	commandBuffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eAccelerationStructureBuildNV,
		vk::PipelineStageFlagBits::eAccelerationStructureBuildNV,
		vk::DependencyFlags(),
		{ memoryBarrier },
		{ }, // buffer memory barriers
		{ }  // image memory barriers
	);
}
