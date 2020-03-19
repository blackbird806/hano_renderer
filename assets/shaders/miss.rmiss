#version 460
#extension GL_NV_ray_tracing : require

struct hitPayload
{
	vec3 hitValue;
};

layout(location = 0) rayPayloadInNV hitPayload prd;

void main()
{
	prd.hitValue = vec3(0, 0, 0);
}
