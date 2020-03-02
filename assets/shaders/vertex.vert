#version 460

// layout(binding = 0) readonly uniform UniformBufferObjectStruct { UniformBufferObject Camera; };
// layout(binding = 1) readonly buffer MaterialArray { Material[] Materials; };

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 InTexCoord;
layout(location = 3) in int InMaterialIndex;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec3 FragNormal;
layout(location = 2) out vec2 FragTexCoord;
layout(location = 3) out flat int FragMaterialIndex;


void main()
{
//	Material m = Materials[InMaterialIndex];
//
//    gl_Position = Camera.Projection * Camera.ModelView * vec4(InPosition, 1.0);
//    FragColor = m.Diffuse.xyz;
//	FragNormal = vec3(Camera.ModelView * vec4(InNormal, 0.0)); // technically not correct, should be ModelInverseTranspose
//	FragTexCoord = InTexCoord;
//	FragMaterialIndex = InMaterialIndex;
}