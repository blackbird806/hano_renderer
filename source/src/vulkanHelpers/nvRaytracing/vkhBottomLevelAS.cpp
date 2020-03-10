#include <vulkanHelpers/nvRaytracing/vkhBottomLevelAS.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

namespace
{
	vk::AccelerationStructureCreateInfoNV getCreateInfo(std::vector<vk::GeometryNV> const& geometries, bool allowUpdate) noexcept
	{
		auto const flags = allowUpdate
			? vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate
			: vk::BuildAccelerationStructureFlagBitsNV::ePreferFastTrace;

		vk::AccelerationStructureCreateInfoNV structureInfo = {};
		structureInfo.pNext = nullptr;
		structureInfo.compactedSize = 0;
		structureInfo.info.type = vk::AccelerationStructureTypeNV::eBottomLevel;
		structureInfo.info.flags = flags;
		structureInfo.info.instanceCount = 0; // The bottom-level AS can only contain explicit geometry, and no instances
		structureInfo.info.geometryCount = static_cast<uint32_t>(geometries.size());
		structureInfo.info.pGeometries = geometries.data();

		return structureInfo;
	}
}

BottomLevelAS::BottomLevelAS(vkh::Device const& device_, std::vector<vk::GeometryNV> const& geometries_, bool allowUpdate)
	: AccelerationStructure(device_, getCreateInfo(geometries_, allowUpdate)), geometries(geometries_)
{

}

void BottomLevelAS::generate(vk::CommandBuffer commandBuffer)
{
	auto const memRequirements = getMemoryRequirements();
	resultBuffer = std::make_unique<vkh::Buffer>(*device, memRequirements.result.size, vk::BufferUsageFlagBits::eRayTracingNV, vk::MemoryPropertyFlagBits::eDeviceLocal);
	scratchBuffer = std::make_unique<vkh::Buffer>(*device, memRequirements.build.size, vk::BufferUsageFlagBits::eRayTracingNV, vk::MemoryPropertyFlagBits::eDeviceLocal);

	// Bind the acceleration structure descriptor to the actual memory that will contain it
	vk::BindAccelerationStructureMemoryInfoNV bindInfo = {};
	bindInfo.pNext = nullptr;
	bindInfo.accelerationStructure = handle.get();
	bindInfo.memory = resultBuffer->memory.get();
	bindInfo.memoryOffset = 0;
	bindInfo.deviceIndexCount = 0;
	bindInfo.pDeviceIndices = nullptr;

	device->handle.bindAccelerationStructureMemoryNV({ bindInfo });

	// Build the actual bottom-level acceleration structure
	auto const flags = allowUpdate
		? vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate
		: vk::BuildAccelerationStructureFlagBitsNV::ePreferFastTrace;

	vk::AccelerationStructureInfoNV buildInfo = {};
	buildInfo.pNext = nullptr;
	buildInfo.flags = flags;
	buildInfo.type = vk::AccelerationStructureTypeNV::eBottomLevel;
	buildInfo.instanceCount = 0;
	buildInfo.geometryCount = static_cast<uint32_t>(geometries.size());
	buildInfo.pGeometries = geometries.data();

	commandBuffer.buildAccelerationStructureNV(buildInfo, nullptr, 0, false, handle.get(), nullptr, scratchBuffer->handle.get(), 0);
}

void BottomLevelAS::update(vk::CommandBuffer commandBuffer)
{
	assert(allowUpdate);

	// Bind the acceleration structure descriptor to the actual memory that will contain it
	vk::BindAccelerationStructureMemoryInfoNV bindInfo = {};
	bindInfo.pNext = nullptr;
	bindInfo.accelerationStructure = handle.get();
	bindInfo.memory = resultBuffer->memory.get();
	bindInfo.memoryOffset = 0;
	bindInfo.deviceIndexCount = 0;
	bindInfo.pDeviceIndices = nullptr;

	device->handle.bindAccelerationStructureMemoryNV({ bindInfo });

	// Build the actual bottom-level acceleration structure
	auto const flags = allowUpdate
		? vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate
		: vk::BuildAccelerationStructureFlagBitsNV::ePreferFastTrace;

	vk::AccelerationStructureInfoNV buildInfo = {};
	buildInfo.pNext = nullptr;
	buildInfo.flags = flags;
	buildInfo.type = vk::AccelerationStructureTypeNV::eBottomLevel;
	buildInfo.instanceCount = 0;
	buildInfo.geometryCount = static_cast<uint32_t>(geometries.size());
	buildInfo.pGeometries = geometries.data();

	commandBuffer.buildAccelerationStructureNV(buildInfo, nullptr, 0, false, handle.get(), nullptr, scratchBuffer->handle.get(), 0);
}
