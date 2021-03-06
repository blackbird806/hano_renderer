#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "vkhAccelerationStructure.hpp"
#include "vkhBottomLevelAS.hpp"

namespace hano::vkh
{
	/// Geometry instance, with the layout expected by VK_NV_ray_tracing
	struct GeometryInstance
	{
		/// Transform matrix, containing only the top 3 rows
		float transform[12];
		/// Instance index
		uint32_t instanceId : 24;
		/// Visibility mask
		uint32_t mask : 8;
		/// Index of the hit group which will be invoked when a ray hits the instance
		uint32_t hitGroupId : 24;
		/// Instance flags, such as culling
		uint32_t flags : 8;
		/// Opaque handle of the bottom-level acceleration structure
		uint64_t accelerationStructureHandle;
	};

	struct TopLevelAS final : AccelerationStructure
	{
		TopLevelAS(vkh::Device const& device_, uint32_t instancesCount, bool allowUpdate);

		void generate(vk::CommandBuffer commandBuffer, std::vector<GeometryInstance> const& geometryInstances);
		void update(vk::CommandBuffer commandBuffer, std::vector<GeometryInstance> const& geometryInstancess);

		static GeometryInstance createGeometryInstance(
			BottomLevelAS const& bottomLevelAs,
			glm::mat4 const& transform,
			uint32_t instanceId,
			uint32_t hitGroupIndex);

	private:

		std::unique_ptr<vkh::Buffer> resultBuffer;
		std::unique_ptr<vkh::Buffer> instancesBuffer;
		std::unique_ptr<vkh::Buffer> scratchBuffer;
	};
}