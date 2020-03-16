#version 460

layout(binding = 2) uniform sampler2D TextureSamplers;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in flat int fragMaterialIndex;

layout(location = 0) out vec4 outColor;

void main() 
{
	// const int textureId = Materials[fragMaterialIndex].DiffuseTextureId;
	// const vec3 lightVector = normalize(vec3(5, 4, 3));
	// const float d = max(dot(lightVector, normalize(FragNormal)), 0.2);
	
	// vec3 c = FragColor * d;
	// if (textureId >= 0)	
	// {
	// 	c *= texture(TextureSamplers[textureId], fragTexCoord).rgb;
	// }

    outColor = vec4(1, 0, 0, 1);
}