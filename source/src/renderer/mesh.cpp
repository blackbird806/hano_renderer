#include <renderer/mesh.hpp>
#include <core/logger.hpp>

using namespace hano;

Mesh::Mesh(std::vector<Vertex> const& vertices, std::vector<uint32> const& indices)
{
	setVertices(vertices);
	setIndices(indices);
}

void Mesh::setVertices(std::vector<Vertex> const& vertices)
{
	if (vertices.empty())
	{
		warnf("trying to create mesh without vertices !");
		return;
	}

	m_vertexCount = static_cast<uint32>(vertices.size());
	vkh::Buffer staging();
}

void Mesh::setIndices(std::vector<uint32> const& indices)
{
	assert(false);
}

std::vector<Vertex> Mesh::getVertices()
{
	assert(false);
	return std::vector<Vertex>();
}
