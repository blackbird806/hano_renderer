#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in int inMaterialIndex;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out flat int fragMaterialIndex;

void main()
{
   	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
	fragNormal = vec3(ubo.view * inverse(transpose(ubo.model)) * vec4(inNormal, 0.0)); 
   	// fragColor = m.Diffuse.xyz;
	// fragTexCoord = inTexCoord;
	// fragMaterialIndex = inMaterialIndex;
}