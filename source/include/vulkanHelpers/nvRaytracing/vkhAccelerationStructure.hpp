#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkanHelpers/vkhUtility.hpp>

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

		VULKAN_NON_COPIABLE(AccelerationStructure);

		MemoryRequirements getMemoryRequirements() const;
		static void memoryBarrier(vk::CommandBuffer commandBuffer);

		vkh::Device const* device;
		vk::UniqueAccelerationStructureNV handle;
		
		protected:

			AccelerationStructure(vkh::Device const& device_, vk::AccelerationStructureCreateInfoNV const& createInfo);
			bool const allowUpdate;
	};
}