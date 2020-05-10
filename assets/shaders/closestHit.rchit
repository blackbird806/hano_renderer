#version 460
#extension GL_NV_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable // see : https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_nonuniform_qualifier.txt

#define ENABLE_SHADOWS 1

#define NUM_MAX_LIGHTS 4

struct hitPayload
{
	vec3 hitValue;
};

struct Light
{
	float intensity;
	vec3 pos;
	vec3 color;
};

hitAttributeNV vec3 attribs;

layout(push_constant) uniform PushConsts {
	uint nbLights;
} pushConsts;

layout(location = 0) rayPayloadInNV hitPayload prd;
layout(location = 1) rayPayloadInNV bool isShadowed;

layout(binding = 0, set = 0) uniform accelerationStructureNV topLevelAS;

layout(binding = 3, set = 0) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 4, set = 0) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 5, set = 0) readonly buffer OffsetArray { uvec2 Offsets[]; };
layout(binding = 6, set = 0) uniform LightUBO { Light lights[NUM_MAX_LIGHTS]; };
layout(binding = 8, set = 0) uniform sampler2D textureSamplers[];

struct Vertex
{
	vec3 pos;
	vec3 normal;
	vec2 texCoord;
	int matIndex;
};

Vertex unpackVertex(uint index)
{
	const uint vertexSize = 9;
	const uint offset = index * vertexSize;
	
	Vertex v;
	
	v.pos = vec3(Vertices[offset + 0], Vertices[offset + 1], Vertices[offset + 2]);
	v.normal = vec3(Vertices[offset + 3], Vertices[offset + 4], Vertices[offset + 5]);
	v.texCoord = vec2(Vertices[offset + 6], Vertices[offset + 7]);
	v.matIndex = floatBitsToInt(Vertices[offset + 8]);

	return v;
}

vec2 Mix(vec2 a, vec2 b, vec2 c, vec3 barycentrics)
{
	return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

vec3 Mix(vec3 a, vec3 b, vec3 c, vec3 barycentrics) 
{
    return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

void main()
{
	const uvec2 offset = Offsets[gl_InstanceCustomIndexNV];
	const uint vertexOffset = offset.x;
	const uint indexOffset = offset.y;

	const Vertex v0 = unpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 0]);
	const Vertex v1 = unpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 1]);
	const Vertex v2 = unpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 2]);

  	const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
	vec3 normal = normalize(Mix(v0.normal, v1.normal, v2.normal, barycentrics));
	const mat4 model = mat4(gl_ObjectToWorldNV);
	
	// Transforming the normal to world space
	normal = normalize(vec3(inverse(transpose(model)) * vec4(normal, 0.0)));
	const vec2 texCoord = Mix(v0.texCoord, v1.texCoord, v2.texCoord, barycentrics);

	vec3 worldPos = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV; // may have precision issues with far hitpoints
  	// vec3 worldPos = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;

	// light
	vec3 sum = vec3(0.01, 0.01, 0.01);
	isShadowed = false;

	for (int i = 0; i < pushConsts.nbLights; i++)
	{
		vec3 lightPos = lights[i].pos;
		vec3 lightColor = lights[i].color;
		float lightIntensity = lights[i].intensity;
		
		const vec3 lightDir = normalize(lightPos - worldPos);
		const float lightDist = length(lightDir);

		const float U = dot(normal, lightDir);

		// shadow cast
		if (U > 0)
		{
#if ENABLE_SHADOWS
			float tMin   = 0.001;
			float tMax   = lightDist;
			vec3  origin = worldPos;
			vec3  rayDir = lightDir;
			uint  flags = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV;
			isShadowed = true;
			traceNV(topLevelAS,  // acceleration structure
					flags,       // rayFlags
					0xFF,        // cullMask
					0,           // sbtRecordOffset
					0,           // sbtRecordStride
					1,           // missIndex
					origin,      // ray origin
					tMin,        // ray min range
					rayDir,      // ray direction
					tMax,        // ray max range
					1            // payload (location = 1) isShadowed
			);
#endif
			if (isShadowed)
			{
				sum += 0.25 * lightColor * lightIntensity;
			}
			else
			{
				sum += max(U, 0.2) * lightColor * lightIntensity;
			}
		}
	}
	// ---

	vec4 texColor = texture(textureSamplers[gl_InstanceCustomIndexNV], texCoord);
	// vec3 color = vec3(texCoord.x, texCoord.y, 0.5);
	// vec3 color = normal;

			// float tMin   = 0.001;
			// float tMax   = 1000.0;
			// vec3  origin = worldPos;
			// vec3  rayDir = reflect(gl_WorldRayDirectionNV, normal);
			// uint  flags = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV;
			// traceNV(topLevelAS,  // acceleration structure
			// 		flags,       // rayFlags
			// 		0xFF,        // cullMask
			// 		0,           // sbtRecordOffset
			// 		0,           // sbtRecordStride
			// 		0,           // missIndex
			// 		origin,      // ray origin
			// 		tMin,        // ray min range
			// 		rayDir,      // ray direction
			// 		tMax,        // ray max range
			// 		0            // payload (location = 1) isShadowed
			// );

	vec3 color = sum * texColor.xyz;

	prd.hitValue = color;
}
