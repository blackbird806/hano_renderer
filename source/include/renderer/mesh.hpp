#pragma once

#include <vector>
#include <filesystem>
#include <vulkan/vulkan.hpp>

#include <core/hanoConfig.hpp>
#include <vulkanHelpers/vkhBuffer.hpp>
#include <vulkanHelpers/vkhDescriptorSets.hpp>
#include "vulkanContext.hpp"
#include "vertex.hpp"

namespace tinygltf
{
	class Model;
}

namespace hano
{
	class Mesh
	{
		public:

			Mesh(VulkanContext& context);
			Mesh(VulkanContext& context, std::filesystem::path const& objPath);
			Mesh(VulkanContext& context, std::vector<Vertex> const& vertices, std::vector<uint32> const& indices = {});
			
			Mesh(Mesh&&) noexcept = default;
			Mesh& operator=(Mesh&&) noexcept = default;

			void setVertices(std::vector<Vertex> const& vertices);
			void setIndices(std::vector<uint32> const& indices);

			vk::GeometryNV toVkGeometryNV() const;

			void render(vk::CommandBuffer commandBuffer, uint32 instances = 1) const;
			std::vector<Vertex> getVerticesFromDeviceMemory();
			std::vector<Vertex> const& getVertices() const;
			std::vector<uint32> const& getIndices() const;

			void loadObj(std::filesystem::path const& objPath);
			void loadFromGltfFile(std::filesystem::path const& gltfPath);

			// load mesh from already openned gltf file
			void loadGltf(tinygltf::Model const& model);

		private:

			VulkanContext* m_vkContext;

			std::vector<Vertex> m_vertices;
			std::vector<uint32> m_indices;
			
			vkh::Buffer m_vertexBuffer;
			vkh::Buffer m_indexBuffer;

			uint32 m_vertexCount;
			uint32 m_indexCount;
	};
}