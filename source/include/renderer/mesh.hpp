#pragma once

#include <vector>
#include <filesystem>
#include <vulkan/vulkan.hpp>
#include <vulkanHelpers/vkhBuffer.hpp>
#include <core/hanoConfig.hpp>
#include "vertex.hpp"

namespace hano
{
	class Mesh
	{
		public:

			Mesh(std::filesystem::path const& objPath);
			Mesh(std::vector<Vertex> const& vertices, std::vector<uint32> const& indices = {});
			Mesh(Mesh&&) noexcept;

			Mesh& operator=(Mesh&&) noexcept;

			void setVertices(std::vector<Vertex> const& vertices);
			void setIndices(std::vector<uint32> const& indices);

			void render(vk::CommandBuffer commandBuffer, uint32 instances = 1) const;

			// get vertices from GPU memory
			std::vector<Vertex> getVertices();

		private:

			void loadObj(std::filesystem::path const& objPath);

			vkh::Buffer m_vertexBuffer;
			vkh::Buffer m_indexBuffer;

			uint32 m_vertexCount;
			uint32 m_indexCount;
	};
}