#pragma once

#include <vector>
#include <vulkanHelpers/vkhBuffer.hpp>
#include "vkhAccelerationStructure.hpp"

namespace hano::vkh
{
	struct BottomLevelAS final : AccelerationStructure
	{
		BottomLevelAS(vkh::Device const& device_, std::vector<vk::GeometryNV> const& geometries_, bool allowUpdate);
		
		void generate(vk::CommandBuffer commandBuffer);
		void update(vk::CommandBuffer commandBuffer);

		private:
			
			std::vector<vk::GeometryNV> geometries;
			std::unique_ptr<vkh::Buffer> resultBuffer;
			std::unique_ptr<vkh::Buffer> scratchBuffer;
	};
}