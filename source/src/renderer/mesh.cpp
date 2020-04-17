#include <renderer/mesh.hpp>

#include <tiny/tiny_obj_loader.h>
#include <tiny/tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>

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
	else if (filePath.extension() == ".gltf")
		loadGltf(filePath);
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

// @Review Move vertices and indices may avoid copy 
void Mesh::setVertices(std::vector<Vertex> const& vertices)
{
	if (vertices.empty())
	{
		hano_warnf("trying to create mesh without vertices !");
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
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc, 
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	m_vertexBuffer.copyFrom(*m_vkContext->commandPool, staging, bufferSize);
	// @TODO optional keep
	m_vertices = vertices;	
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
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	m_indexBuffer.copyFrom(*m_vkContext->commandPool, staging, bufferSize);
	// @TODO optional keep
	m_indices = indices;
}

// @TO TEST
std::vector<Vertex> Mesh::getVerticesFromDeviceMemory()
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
	 // @TODO use obj v2 API
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

void Mesh::loadGltf(std::filesystem::path const& gltfPath)
{
	using namespace tinygltf;

	std::vector<Vertex> vertices;
	std::vector<uint32> indices;

	Model model;
	TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, gltfPath.string());
	//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

	if (!warn.empty()) {
		hano_warnf("Warn: %s\n", warn.c_str());
	}

	if (!err.empty()) {
		hano_errorf("Err: %s\n", err.c_str());
	}

	if (!ret) {
		throw HanoException("Failed to parse glTF\n");
	}

	auto mesh = *model.meshes.begin(); // when loading gltf as mesh, only consider first mesh found

	for (int i = 0; i < mesh.primitives.size(); i++)
	{
		auto const& primitive = mesh.primitives[i];

		uint32_t const indexStart = 0;
		uint32_t const vertexStart = 0;

		Accessor const& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
		BufferView const& posView = model.bufferViews[posAccessor.bufferView];
		float const* bufferPos = reinterpret_cast<float const*>(&model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]);
		int posByteStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

		int normByteStride;
		float const* bufferNormals;
		if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
			tinygltf::Accessor const& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
			tinygltf::BufferView const& normView = model.bufferViews[normAccessor.bufferView];
			bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
			normByteStride = normAccessor.ByteStride(normView) ? (normAccessor.ByteStride(normView) / sizeof(float)) : GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
		}

		int uv0ByteStride;
		float const* bufferTexCoordSet0;
		if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
			tinygltf::Accessor const& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
			tinygltf::BufferView const& uvView = model.bufferViews[uvAccessor.bufferView];
			bufferTexCoordSet0 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
			uv0ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
		}
#if 0 // @TODO
		int uv1ByteStride;
		float const* bufferTexCoordSet1;
		if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
			tinygltf::Accessor const& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
			tinygltf::BufferView const& uvView = model.bufferViews[uvAccessor.bufferView];
			bufferTexCoordSet1 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
			uv1ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
		}
#endif
		for (size_t v = 0; v < posAccessor.count; v++) {
			Vertex vert{};
			vert.pos = glm::make_vec3(&bufferPos[v * posByteStride]);
			vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * normByteStride]) : glm::vec3(0.0f)));
			vert.texCoord = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * uv0ByteStride]) : glm::vec3(0.0f);
#if 0
			vert.uv1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * uv1ByteStride]) : glm::vec3(0.0f);
#endif
			vertices.push_back(vert);
		}

		if (primitive.indices > -1)
		{
			tinygltf::Accessor const& accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
			tinygltf::BufferView const& bufferView = model.bufferViews[accessor.bufferView];
			tinygltf::Buffer const& buffer = model.buffers[bufferView.buffer];

			const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);
			switch (accessor.componentType) {
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
				const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
				for (size_t index = 0; index < accessor.count; index++) {
					indices.push_back(buf[index] + vertexStart);
				}
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
				const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
				for (size_t index = 0; index < accessor.count; index++) {
					indices.push_back(buf[index] + vertexStart);
				}
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
				const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
				for (size_t index = 0; index < accessor.count; index++) {
					indices.push_back(buf[index] + vertexStart);
				}
				break;
			}
			default:
				hano_error_stream << "Index component type " << accessor.componentType << " not supported!" << std::endl;
				return;
			}
		}

	}
	setVertices(vertices);
	setIndices(indices);
}


std::vector<Vertex> const& Mesh::getVertices() const
{
	if (m_vertices.empty())
		throw HanoException("no vertex buffer in host memory !");

	return m_vertices;
}

std::vector<uint32> const& Mesh::getIndices() const
{
	if (m_indices.empty())
		throw HanoException("no index buffer in host memory !");

	return m_indices;
}
