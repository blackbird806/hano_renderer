#pragma once

#include <vector>
#include <filesystem>
#include <vulkan/vulkan.hpp>

#include <core/hanoConfig.hpp>
#include <vulkanHelpers/vkhBuffer.hpp>
#include <vulkanHelpers/vkhDescriptorSets.hpp>
#include "vulkanContext.hpp"
#include "vertex.hpp"

namespace hano
{
	class Mesh
	{
		public:

			Mesh(VulkanContext& context, std::filesystem::path const& objPath);
			Mesh(VulkanContext& context, std::vector<Vertex> const& vertices, std::vector<uint32> const& indices = {});
			
			Mesh(Mesh&&) noexcept = default;
			Mesh& operator=(Mesh&&) noexcept = default;

			void setVertices(std::vector<Vertex> const& vertices);
			void setIndices(std::vector<uint32> const& indices);

			vk::GeometryNV toVkGeometryNV() const;

			void render(vk::CommandBuffer commandBuffer, uint32 instances = 1);

			// get vertices from GPU memory
			std::vector<Vertex> getVertices();

		private:

			VulkanContext* m_vkContext;

			void loadObj(std::filesystem::path const& objPath);

			vkh::Buffer m_vertexBuffer;
			vkh::Buffer m_indexBuffer;

			uint32 m_vertexCount;
			uint32 m_indexCount;
	};
}