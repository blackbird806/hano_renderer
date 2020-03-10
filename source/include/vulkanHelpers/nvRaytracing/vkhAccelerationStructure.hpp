#pragma once

#include <vulkan/vulkan.hpp>

namespace hano::vkh
{
	struct Device;

	struct AccelerationStructure
	{
		struct MemoryRequirements
		{
			vk::MemoryRequirements result;
			vk::MemoryRequirements build;
			vk::MemoryRequirements update;
		};

		MemoryRequirements getMemoryRequirements() const;
		static void memoryBarrier(vk::CommandBuffer commandBuffer);

		vkh::Device const* device;
		vk::UniqueAccelerationStructureNV handle;
		
		protected:

			AccelerationStructure(vkh::Device const& device_, vk::AccelerationStructureCreateInfoNV const& createInfo);
			bool const allowUpdate;
	};
}