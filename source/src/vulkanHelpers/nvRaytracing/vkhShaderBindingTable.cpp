#include <vulkanHelpers/nvRaytracing/vkhShaderBindingTable.hpp>
#include <vulkanHelpers/nvRaytracing/vkhRaytracingPipeline.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#undef max

using namespace hano::vkh;

namespace
{
	constexpr size_t roundUp(size_t size, size_t powerOf2Alignment) noexcept
	{
		return (size + powerOf2Alignment - 1) & ~(powerOf2Alignment - 1);
	}

	size_t getEntrySize(std::vector<ShaderBindingTable::Entry> const& entries, vk::DeviceSize progIdSize)
	{
		// Find the maximum number of parameters used by a single entry
		size_t maxArgs = 0;

		for (const auto& entry : entries)
		{
			maxArgs = std::max(maxArgs, entry.inlineData.size());
		}

		vk::DeviceSize entrySize = progIdSize + static_cast<vk::DeviceSize>(maxArgs);

		// A SBT entry is made of a program ID and a set of 4-byte parameters (offsets or push constants)
		// and must be 16-bytes-aligned.
		return roundUp(entrySize + maxArgs, 16);
	}

	vk::DeviceSize copyShaderData(
		uint8_t* const dst,
		std::vector<ShaderBindingTable::Entry> const& entries,
		vk::DeviceSize entrySize,
		uint8_t* const shaderHandleStorage,
		vk::DeviceSize progIdSize)
	{
		uint8_t* pDst = dst;

		for (const auto& entry : entries)
		{
			// Copy the shader identifier that was previously obtained with vkGetRayTracingShaderGroupHandlesNV.
			std::memcpy(pDst, shaderHandleStorage + entry.groupIndex * progIdSize, progIdSize);
			std::memcpy(pDst + progIdSize, entry.inlineData.data(), entry.inlineData.size());

			pDst += entrySize;
		}

		return entries.size() * entrySize;
	}

}

ShaderBindingTable::ShaderBindingTable(Device const& device_, RaytracingPipeline const& pipeline,
	std::vector<Entry> const& rayGenPrograms,
	std::vector<Entry> const& missPrograms,
	std::vector<Entry> const& hitGroups)
{
	auto properties = device_.physicalDevice.getProperties2<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceRayTracingPropertiesNV>();
	vk::PhysicalDeviceRayTracingPropertiesNV raytracingProps = properties.get<vk::PhysicalDeviceRayTracingPropertiesNV>();
	
	m_buffer = std::make_unique<vkh::Buffer>(
		device_, 
		computeSize(device_, raytracingProps.shaderGroupHandleSize, rayGenPrograms, missPrograms, hitGroups),
		vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible);
		
	uint32_t groupCount = rayGenPrograms.size() + missPrograms.size() + hitGroups.size();

	std::vector<uint8_t> shaderHandleStorage(raytracingProps.shaderGroupHandleSize * groupCount);

	device_.handle.getRayTracingShaderGroupHandlesNV(pipeline.handle.get(), 0, groupCount, shaderHandleStorage.size(), shaderHandleStorage.data());

	uint8_t* mappedData = reinterpret_cast<uint8_t*>(m_buffer->map());

	vk::DeviceSize offset = copyShaderData(mappedData, rayGenPrograms, m_rayGenEntrySize, shaderHandleStorage.data(), raytracingProps.shaderGroupHandleSize);
	mappedData += offset;
	offset = copyShaderData(mappedData, missPrograms, m_missEntrySize, shaderHandleStorage.data(), raytracingProps.shaderGroupHandleSize);
	mappedData += offset;
	offset = copyShaderData(mappedData, hitGroups, m_hitGroupEntrySize, shaderHandleStorage.data(), raytracingProps.shaderGroupHandleSize);
	
	m_buffer->unMap();
}

vk::DeviceSize ShaderBindingTable::computeSize(vkh::Device const& device_,
	vk::DeviceSize progIdSize,
	std::vector<Entry> const& rayGenPrograms,
	std::vector<Entry> const& missPrograms,
	std::vector<Entry> const& hitGroups)
{
	// Compute the entry size of each program type depending on the maximum number of parameters in
	// each category
	m_rayGenEntrySize = getEntrySize(rayGenPrograms, progIdSize);
	m_missEntrySize = getEntrySize(missPrograms, progIdSize);
	m_hitGroupEntrySize = getEntrySize(hitGroups, progIdSize);

	// The total SBT size is the sum of the entries for ray generation, miss and hit groups
	auto sbtSize = m_rayGenEntrySize * rayGenPrograms.size()
		+ m_missEntrySize * missPrograms.size()
		+ m_hitGroupEntrySize * hitGroups.size();

	return sbtSize;
}

Buffer const& ShaderBindingTable::getBuffer() const noexcept
{
	return *m_buffer;
}