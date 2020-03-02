#include <renderer/mesh.hpp>
#include <vulkanContext.hpp>
#include <vulkanHelpers/vkhCommandBuffers.hpp>
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
