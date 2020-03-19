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

	const vec3 lightVector = normalize(vec3(5, 4, 3));
	const float intensity = max(dot(lightVector, normalize(fragNormal)), 0.0);

	// vec4 color;
	// float fraction = 1.0;
	//  if (intensity > pow(0.95, fraction)) {
	// 		color = vec4(vec3(1.0), 1.0);
	// } else if (intensity > pow(0.5, fraction)) {
	// 		color = vec4(vec3(0.6), 1.0);
	// } else if (intensity > pow(0.25, fraction)) {
	// 		color = vec4(vec3(0.4), 1.0);
	// } else {
	// 		color = vec4(vec3(0.2), 1.0);
	// }


	// vec3 c = FragColor * d;
	// if (textureId >= 0)	
	// {
	// 	c *= texture(TextureSamplers[textureId], fragTexCoord).rgb;
	// }

    outColor = vec4(0, 1, 0, 1);
}