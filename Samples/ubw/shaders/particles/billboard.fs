#version 330

uniform sampler2D diffuseSampler;
uniform vec4 colorMultiplier;

in vec2 texCoord;
in vec4 position;

layout(location = 0) out vec4 colorOut;
layout(location = 1) out vec4 normalOut;
layout(location = 2) out vec4 diffuseOut;
layout(location = 3) out vec4 positionOut;

void main()
{
    diffuseOut = texture(diffuseSampler, texCoord)*colorMultiplier;
    positionOut = position;
}
