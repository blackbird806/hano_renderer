#pragma once

#include <vector>
#include <vulkanHelpers/vkhBuffer.hpp>
#include "vkhAccelerationStructure.hpp"

namespace hano::vkh
{
	struct BottomLevelAS final : AccelerationStructure
	{
		BottomLevelAS(vkh::Device const& device_, std::vector<vk::GeometryNV> const& geometries_, bool allowUpdate);

		void generate(vk::CommandBuffer commandBuffer, 
			vkh::Buffer& scratchBuffer, vk::DeviceSize scratchOffset, 
			vkh::Buffer& resultBuffer, vk::DeviceSize resultOffset, 
			bool updateOnly);

		private:

			std::vector<vk::GeometryNV> geometries;
	};
}