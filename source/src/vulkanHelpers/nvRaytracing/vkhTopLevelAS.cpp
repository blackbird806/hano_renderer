#include <vulkanHelpers/nvRaytracing/vkhTopLevelAS.hpp>
#include <vulkanHelpers/vkhDevice.hpp>

using namespace hano::vkh;

namespace 
{
	vk::AccelerationStructureCreateInfoNV getCreateInfo(uint32_t instanceCount, bool allowUpdate)
	{
		auto const flags = allowUpdate
			? vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate
			: vk::BuildAccelerationStructureFlagBitsNV::ePreferFastTrace;

		vk::AccelerationStructureCreateInfoNV structureInfo = {};
		structureInfo.pNext = nullptr;
		structureInfo.info.type = vk::AccelerationStructureTypeNV::eTopLevel;
		structureInfo.info.flags = flags;
		structureInfo.compactedSize = 0;
		structureInfo.info.instanceCount = instanceCount;
		structureInfo.info.geometryCount = 0; // Since this is a top-level AS, it does not contain any geometry
		structureInfo.info.pGeometries = nullptr;

		return structureInfo;
	}
}

TopLevelAS::TopLevelAS(vkh::Device const& device_, std::vector<GeometryInstance> const& instances, bool allowUpdate_)
	: AccelerationStructure(device_, getCreateInfo(instances.size(), allowUpdate_)), geometryInstances(instances)
{

}

void TopLevelAS::generate(vk::CommandBuffer commandBuffer)
{
	auto const memRequirements = getMemoryRequirements();
	// Copy the instance descriptors into the provider buffer.
	auto const instancesBufferSize = geometryInstances.size() * sizeof(GeometryInstance);

	resultBuffer = std::make_unique<vkh::Buffer>(*device, memRequirements.result.size, vk::BufferUsageFlagBits::eRayTracingNV, vk::MemoryPropertyFlagBits::eDeviceLocal);
	scratchBuffer = std::make_unique<vkh::Buffer>(*device, memRequirements.build.size, vk::BufferUsageFlagBits::eRayTracingNV, vk::MemoryPropertyFlagBits::eDeviceLocal);
	instancesBuffer = std::make_unique<vkh::Buffer>(*device, instancesBufferSize, vk::BufferUsageFlagBits::eRayTracingNV, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* data = instancesBuffer->map();
	std::memcpy(data, geometryInstances.data(), instancesBufferSize);
	instancesBuffer->unMap();

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
	buildInfo.type = vk::AccelerationStructureTypeNV::eTopLevel;
	buildInfo.instanceCount = static_cast<uint32_t>(geometryInstances.size());
	buildInfo.geometryCount = 0;
	buildInfo.pGeometries = nullptr;
	
	commandBuffer.buildAccelerationStructureNV(buildInfo, instancesBuffer->handle.get(), 0, VK_FALSE, handle.get(), nullptr, scratchBuffer->handle.get(), 0);
}

void TopLevelAS::update(vk::CommandBuffer commandBuffer)
{
	assert(allowUpdate);

	auto const instancesBufferSize = geometryInstances.size() * sizeof(GeometryInstance);
	void* data = instancesBuffer->map();
	std::memcpy(data, geometryInstances.data(), instancesBufferSize);
	instancesBuffer->unMap();

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
	buildInfo.type = vk::AccelerationStructureTypeNV::eTopLevel;
	buildInfo.instanceCount = static_cast<uint32_t>(geometryInstances.size());
	buildInfo.geometryCount = 0;
	buildInfo.pGeometries = nullptr;

	commandBuffer.buildAccelerationStructureNV(buildInfo, instancesBuffer->handle.get(), 0, VK_TRUE, handle.get(), handle.get(), scratchBuffer->handle.get(), 0);
}

GeometryInstance TopLevelAS::createGeometryInstance(
	BottomLevelAS const& bottomLevelAs,
	glm::mat4 const& transform,
	uint32_t instanceId,
	uint32_t hitGroupIndex)
{
	uint64_t accelerationStructureHandle;
	bottomLevelAs.device->handle.getAccelerationStructureHandleNV(bottomLevelAs.handle.get(), sizeof(accelerationStructureHandle), &accelerationStructureHandle);

	GeometryInstance geometryInstance = {};
	std::memcpy(geometryInstance.transform, &transform, sizeof(glm::mat4));
	geometryInstance.instanceCustomIndex = instanceId;
	geometryInstance.mask = 0xFF; // The visibility mask is always set of 0xFF, but if some instances would need to be ignored in some cases, this flag should be passed by the application.
	geometryInstance.instanceOffset = hitGroupIndex; // Set the hit group index, that will be used to find the shader code to execute when hitting the geometry.
	geometryInstance.flags = (uint32_t)vk::GeometryInstanceFlagBitsNV::eTriangleFrontCounterclockwise;
	geometryInstance.accelerationStructureHandle = accelerationStructureHandle;

	return geometryInstance;
}
