#version 460
#extension GL_NV_ray_tracing : require

struct hitPayload
{
	vec3 hitValue;
};

hitAttributeNV vec3 attribs;

layout(location = 0) rayPayloadInNV hitPayload prd;

layout(binding = 0, set = 0) uniform accelerationStructureNV topLevelAS;

void main()
{
	prd.hitValue = vec3(0, 1, 0);
}
