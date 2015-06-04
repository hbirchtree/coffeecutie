#version 330

uniform sampler2D diffuseSampler;
uniform vec4 colorMultiplier;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture2D(diffuseSampler, TexCoord)*colorMultiplier;
}
