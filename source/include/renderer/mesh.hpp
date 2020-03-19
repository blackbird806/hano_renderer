#pragma once

#include <vector>
#include <filesystem>
#include <vulkan/vulkan.hpp>

#include <core/hanoConfig.hpp>
#include <vulkanHelpers/vkhBuffer.hpp>
#include <vulkanHelpers/vkhDescriptorSets.hpp>
#include "vulkanContext.hpp"
#include "vertex.hpp"
#include "transform.hpp"
#include "camera.hpp"

namespace hano
{
	class Mesh
	{
		public:

			Mesh(VulkanContext& context, std::filesystem::path const& objPath);
			Mesh(VulkanContext& context, std::vector<Vertex> const& vertices, std::vector<uint32> const& indices = {});
			Mesh(Mesh&&) noexcept;

			Mesh& operator=(Mesh&&) noexcept;

			void setVertices(std::vector<Vertex> const& vertices);
			void setIndices(std::vector<uint32> const& indices);

			vk::GeometryNV toVkGeometryNV() const;

			void handleResizing();

			void updateUniformBuffer(uint32 currentFrame, Camera const& camera);
			void render(vk::CommandBuffer commandBuffer, Camera const& camera, uint32 instances = 1);

			// get vertices from GPU memory
			std::vector<Vertex> getVertices();

			Transform transform;

		private:

			VulkanContext* m_vkContext;

			void loadObj(std::filesystem::path const& objPath);
			void createUniformBuffers();

			vkh::Buffer m_vertexBuffer;
			vkh::Buffer m_indexBuffer;

			std::vector<vkh::Buffer> m_uniformBuffers;
			vkh::DescriptorSets m_descriptorSets;

			uint32 m_vertexCount;
			uint32 m_indexCount;
	};
}