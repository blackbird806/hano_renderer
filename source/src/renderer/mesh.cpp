#include <renderer/mesh.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny/tiny_obj_loader.h>
#include <vulkanContext.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>
#include <core/logger.hpp>

using namespace hano;

Mesh::Mesh(std::filesystem::path const& filePath)
{
	if (filePath.extension() == ".obj")
		loadObj(filePath);
}

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

	uint32 const bufferSize = sizeof(Vertex) * vertices.size();

	vkh::Buffer staging(VulkanContext::getDevice(), bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc, 
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* mappedMem = staging.map();
	memcpy(mappedMem, vertices.data(), bufferSize);
	staging.unMap();

	m_vertexBuffer = std::make_unique<vkh::Buffer>(VulkanContext::getDevice(), bufferSize,
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, // VK_IMAGE_USAGE_TRANSFER_SRC_BIT ?
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	m_vertexBuffer->copyFrom(VulkanContext::getCommandPool(), staging, bufferSize);
}

void Mesh::setIndices(std::vector<uint32> const& indices)
{
	if (indices.empty())
		return;

	m_indexCount = static_cast<uint32>(indices.size());

	uint32 const bufferSize = sizeof(Vertex) * indices.size();

	vkh::Buffer staging(VulkanContext::getDevice(), bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* mappedMem = staging.map();
	memcpy(mappedMem, indices.data(), bufferSize);
	staging.unMap();

	m_indexBuffer = std::make_unique<vkh::Buffer>(VulkanContext::getDevice(), bufferSize,
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, // VK_IMAGE_USAGE_TRANSFER_SRC_BIT ?
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	m_indexBuffer->copyFrom(VulkanContext::getCommandPool(), staging, bufferSize);
}

std::vector<Vertex> Mesh::getVertices()
{
	assert(false);
	return std::vector<Vertex>();
}

void Mesh::render(vk::CommandBuffer commandBuffer, uint32 instances)
{
	if (m_indexCount > 0)
	{
		commandBuffer.bindIndexBuffer(m_indexBuffer->handle, 0, vk::IndexType::eUint32);
		commandBuffer.bindVertexBuffers(0, { m_vertexBuffer->handle }, { 0 });
		commandBuffer.drawIndexed(m_indexCount, instances, 0, 0, 0);
	}
	else
	{
		commandBuffer.bindVertexBuffers(0, { m_vertexBuffer->handle }, { 0 });
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
}