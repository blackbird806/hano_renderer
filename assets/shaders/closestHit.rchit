#version 460
#extension GL_NV_ray_tracing : require

struct hitPayload
{
	vec3 hitValue;
};

hitAttributeNV vec3 attribs;

layout(location = 0) rayPayloadInNV hitPayload prd;

layout(binding = 0, set = 0) uniform accelerationStructureNV topLevelAS;

layout(binding = 3, set = 0) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 4, set = 0) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 5, set = 0) readonly buffer OffsetArray { uvec2 Offsets[]; };

struct Vertex
{
  vec3 Position;
  vec3 Normal;
  vec2 TexCoord;
  int MaterialIndex;
};

Vertex UnpackVertex(uint index)
{
	const uint vertexSize = 9;
	const uint offset = index * vertexSize;
	
	Vertex v;
	
	v.Position = vec3(Vertices[offset + 0], Vertices[offset + 1], Vertices[offset + 2]);
	v.Normal = vec3(Vertices[offset + 3], Vertices[offset + 4], Vertices[offset + 5]);
	v.TexCoord = vec2(Vertices[offset + 6], Vertices[offset + 7]);
	v.MaterialIndex = floatBitsToInt(Vertices[offset + 8]);

	return v;
}

void main()
{
	const uvec2 offsets = Offsets[gl_InstanceCustomIndexNV];
	const uint indexOffset = offsets.x;
	const uint vertexOffset = offsets.y;
	const Vertex v0 = UnpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 0]);
	const Vertex v1 = UnpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 1]);
	const Vertex v2 = UnpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 2]);

  	const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
	// const vec3 normal = normalize(Mix(v0.Normal, v1.Normal, v2.Normal, barycentrics));
	// const vec2 texCoord = Mix(v0.TexCoord, v1.TexCoord, v2.TexCoord, barycentrics);

	prd.hitValue = vec3(attribs.x, attribs.y, attribs.z);
}
