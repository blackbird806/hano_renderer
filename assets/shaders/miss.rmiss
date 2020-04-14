#version 460
#extension GL_NV_ray_tracing : require
#define M_PI 3.1415926535897932384626433832795

struct hitPayload
{
	vec3 hitValue;
};

layout(location = 0) rayPayloadInNV hitPayload prd;
layout(binding = 7, set = 0) uniform sampler2D envMap;

void main()
{
	const float phi = atan(gl_WorldRayDirectionNV.z, gl_WorldRayDirectionNV.x);
	const float theta = acos(gl_WorldRayDirectionNV.y);
	
	ivec2 textureSize = textureSize(envMap, 0);
	vec2 coord;
	coord.x = (phi / M_PI) / 2.0;
	coord.y = -(theta / M_PI);

	prd.hitValue = texture(envMap, coord).xyz;
	// prd.hitValue = gl_WorldRayDirectionNV; // cool effect
}
