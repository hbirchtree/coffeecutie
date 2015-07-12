#version 330

in vec3 TexCoord0;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragNormal;
layout(location = 2) out vec4 FragDiffuse;

uniform samplerCube cubemapTexture;

void main()
{
    FragDiffuse = texture(cubemapTexture, TexCoord0);
	FragNormal = vec4(0.0,0.0,0.0,0.0);
}
