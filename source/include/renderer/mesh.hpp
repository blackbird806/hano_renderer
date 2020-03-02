#pragma once

#include <memory>
#include <vector>
#include <vulkanHelpers/vkhBuffer.hpp>
#include <core/hanoConfig.hpp>
#include "vertex.hpp"

namespace hano
{
	class Mesh
	{
		public:

			Mesh(std::vector<Vertex> const& vertices, std::vector<uint32> const& indices = {});
			
			void setVertices(std::vector<Vertex> const& vertices);
			void setIndices(std::vector<uint32> const& indices);

			// get vertices from GPU memory
			std::vector<Vertex> getVertices();

		private:

			std::unique_ptr<vkh::Buffer> m_vertexBuffer;
			std::unique_ptr<vkh::Buffer> m_indexBuffer;

			uint32 m_vertexCount;
			uint32 m_indexCount;
	};
}