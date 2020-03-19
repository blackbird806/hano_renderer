#pragma once
#include <memory>
#include <vulkanHelpers/vkhBuffer.hpp>

namespace hano::vkh
{
	struct RaytracingPipeline;
	struct Device;

	// @Review whole
	struct ShaderBindingTable
	{
		struct Entry
		{
			uint32_t groupIndex;
			std::vector<uint8_t> inlineData;
		};

		ShaderBindingTable(Device const& device_, RaytracingPipeline const& pipeline,
							std::vector<Entry> const& rayGenPrograms,
							std::vector<Entry> const& missPrograms,
							std::vector<Entry> const& hitGroups);
		
		vk::DeviceSize  rayGenOffset() const noexcept { return m_rayGenOffset; }
		vk::DeviceSize  missOffset() const noexcept { return m_missOffset; }
		vk::DeviceSize  hitGroupOffset() const noexcept { return m_hitGroupOffset; }

		vk::DeviceSize  rayGenEntrySize() const noexcept { return m_rayGenEntrySize; }
		vk::DeviceSize  missEntrySize() const noexcept { return m_missEntrySize; }
		vk::DeviceSize  hitGroupEntrySize() const noexcept { return m_hitGroupEntrySize; }

		vkh::Buffer const& getBuffer() const noexcept;

		private:

			vk::DeviceSize computeSize(vkh::Device const& device_, 
				vk::DeviceSize progIdSize,
				std::vector<Entry> const& rayGenPrograms,
				std::vector<Entry> const& missPrograms,
				std::vector<Entry> const& hitGroups);

			vk::DeviceSize m_rayGenEntrySize;
			vk::DeviceSize m_missEntrySize;
			vk::DeviceSize m_hitGroupEntrySize;
			
			vk::DeviceSize m_rayGenOffset;
			vk::DeviceSize m_missOffset;
			vk::DeviceSize m_hitGroupOffset;

			std::unique_ptr<vkh::Buffer> m_buffer;
	};
}