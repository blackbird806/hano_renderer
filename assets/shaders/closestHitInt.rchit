#version 460
#extension GL_NV_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable // see : https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_nonuniform_qualifier.txt
#extension GL_EXT_scalar_block_layout : enable

#define ENABLE_SHADOWS 0
#define COMPUTE_LIGHT 1
#define ENABLE_REFLECTION 0
#define NUM_MAX_LIGHTS 4
#define KIND_SPHERE 0
#define KIND_CUBE 1

struct hitPayload
{
	vec3 hitValue;
};

struct Sphere
{
	vec3  center;
	float radius;
};

struct Aabb
{
	vec3 minimum;
	vec3 maximum;
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
layout(binding = 9, set = 0, scalar) buffer allSpheres_
{
  Sphere i[];
} allSpheres;

struct Vertex
{
	vec3 pos;
	vec3 normal;
	vec2 texCoord;
	int matIndex;
};

void main()
{
	Sphere instance = allSpheres.i[gl_PrimitiveID];

	// Transforming the normal to world space
	vec3 worldPos = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV; // may have precision issues with far hitpoints
	// Computing the normal at hit position
	vec3 normal = normalize(worldPos - instance.center);
	if(gl_HitKindNV == KIND_CUBE)  // Aabb
  	{
   		vec3  absN = abs(normal);
    	float maxC = max(max(absN.x, absN.y), absN.z);
    	normal     = (maxC == absN.x) ?
                 	vec3(sign(normal.x), 0, 0) :
                 	(maxC == absN.y) ? vec3(0, sign(normal.y), 0) : vec3(0, 0, sign(normal.z));
  	}

	#if ENABLE_REFLECTION
			float tMin   = 0.001;
			float tMax   = 1000.0;
			vec3  origin = worldPos;
			vec3  rayDir = reflect(gl_WorldRayDirectionNV, normal);
			uint  flags = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV;
			traceNV(topLevelAS,  // acceleration structure
					flags,       // rayFlags
					0xFF,        // cullMask
					0,           // sbtRecordOffset
					0,           // sbtRecordStride
					0,           // missIndex
					origin,      // ray origin
					tMin,        // ray min range
					rayDir,      // ray direction
					tMax,        // ray max range
					0            // payload (location = 1) isShadowed
			);
	#endif

	// light
	vec3 sum = vec3(0.01, 0.01, 0.01);
#if COMPUTE_LIGHT
	isShadowed = false;
	for (int i = 0; i < pushConsts.nbLights; i++)
	{
		vec3 lightPos = lights[i].pos;
		vec3 lightColor = lights[i].color;
		float lightIntensity = lights[i].intensity;
		
		vec3 lightDir = lightPos - worldPos;
		float lightDist = length(lightDir);
		lightDir = normalize(lightDir);

		float U = dot(normal, lightDir);
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
#endif // ENABLE_SHADOWS
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
#endif // COMPUTE_LIGHT

	// ---

	vec3 color = sum + vec3(0.0, 0.0, 0.1);
	prd.hitValue = color;
}
