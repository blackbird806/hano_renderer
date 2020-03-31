#include <renderer/mesh.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny/tiny_obj_loader.h>

#include <vulkanContext.hpp>
#include <vulkanHelpers/vkhDebug.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>
#include <core/logger.hpp>

using namespace hano;

Mesh::Mesh(VulkanContext& context, std::filesystem::path const& filePath)
	: m_vkContext(&context), m_vertexCount(0), m_indexCount(0)
{
	if (filePath.extension() == ".obj")
		loadObj(filePath);
	else
		assert(false);

	vkh::setObjectName(m_vertexBuffer, (filePath.filename().string() + std::string(" vertex buffer")).c_str());
	if (m_indexBuffer)
		vkh::setObjectName(m_indexBuffer, (filePath.filename().string() + std::string(" index buffer")).c_str());
}

Mesh::Mesh(VulkanContext& context, std::vector<Vertex> const& vertices, std::vector<uint32> const& indices)
	: m_vkContext(&context)
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

	uint32 const bufferSize = sizeof(Vertex) * vertices.size();

	vkh::Buffer staging(*m_vkContext->device, bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc, 
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* mappedMem = staging.map();
		memcpy(mappedMem, vertices.data(), bufferSize);
	staging.unMap();

	m_vertexBuffer.init(*m_vkContext->device, bufferSize,
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, // VK_IMAGE_USAGE_TRANSFER_SRC_BIT ?
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	m_vertexBuffer.copyFrom(*m_vkContext->commandPool, staging, bufferSize);
}

void Mesh::setIndices(std::vector<uint32> const& indices)
{
	if (indices.empty())
		return;

	m_indexCount = static_cast<uint32>(indices.size());

	uint32 const bufferSize = sizeof(uint32) * indices.size();

	vkh::Buffer staging(*m_vkContext->device, bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* mappedMem = staging.map();
	memcpy(mappedMem, indices.data(), bufferSize);
	staging.unMap();

	m_indexBuffer.init(*m_vkContext->device, bufferSize,
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, // VK_IMAGE_USAGE_TRANSFER_SRC_BIT ?
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	m_indexBuffer.copyFrom(*m_vkContext->commandPool, staging, bufferSize);
}

// @TO TEST
std::vector<Vertex> Mesh::getVertices()
{
	vkh::Buffer staging(*m_vkContext->device, m_vertexBuffer.getSize(),
		vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	{
		vkh::SingleTimeCommands cmd(*m_vkContext->commandPool);

		vk::BufferCopy copyRegion = {};
		copyRegion.size = staging.getSize();
		cmd.buffer().copyBuffer(m_vertexBuffer.handle.get(), staging.handle.get(), copyRegion);
	}
	
	std::vector<Vertex> vertices(m_vertexCount);
	
	void* data = staging.map();

	memcpy(vertices.data(), data, staging.getSize());

	staging.unMap();

	return vertices;
}

// @Review
vk::GeometryNV Mesh::toVkGeometryNV() const
{
	vk::GeometryTrianglesNV triangles;
	triangles.setVertexData(m_vertexBuffer.handle.get());
	triangles.setVertexOffset(0);  // Start at the beginning of the buffer
	triangles.setVertexCount(m_vertexCount);
	triangles.setVertexStride(sizeof(Vertex));
	triangles.setVertexFormat(vk::Format::eR32G32B32Sfloat);  // 3xfloat32 for vertices
	if (m_indexCount > 0)
	{
		triangles.setIndexData(m_indexBuffer.handle.get());
		triangles.setIndexOffset(0 * sizeof(uint32_t));
		triangles.setIndexCount(m_indexCount);
		triangles.setIndexType(vk::IndexType::eUint32);  // 32-bit indices
	}
	vk::GeometryDataNV geoData;
	geoData.setTriangles(triangles);
	vk::GeometryNV geometry;
	geometry.setGeometry(geoData);
	// Consider the geometry opaque for optimization
	geometry.setFlags(vk::GeometryFlagBitsNV::eOpaque);
	return geometry;
}

void Mesh::render(vk::CommandBuffer commandBuffer, uint32 instances) const
{
	if (m_indexCount > 0)
	{
		commandBuffer.bindIndexBuffer(m_indexBuffer.handle.get(), 0, vk::IndexType::eUint32);
		commandBuffer.bindVertexBuffers(0, { m_vertexBuffer.handle.get() }, { 0 });
		commandBuffer.drawIndexed(m_indexCount, instances, 0, 0, 0);
	}
	else
	{
		commandBuffer.bindVertexBuffers(0, { m_vertexBuffer.handle.get() }, { 0 });
		commandBuffer.draw(m_vertexCount, instances, 0, 0);
	}
}

void Mesh::loadObj(std::filesystem::path const& filePath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.string().c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32> uniqueVertices = {};
	std::vector<Vertex> vertices;
	std::vector<uint32> indices;

	for (const auto& shape : shapes) 
	{
		for (const auto& index : shape.mesh.indices) 
		{
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (!attrib.normals.empty() && index.normal_index >= 0)
			{
				float* np = &attrib.normals[3 * index.normal_index];
				vertex.normal = { np[0], np[1], np[2] };
			}

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	setVertices(vertices);
	setIndices(indices);
}