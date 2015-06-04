#version 330

layout (location = 0) in float type;
layout (location = 1) in vec3 pos;
layout (location = 2) in vec3 vel;
layout (location = 3) in vec3 acc;
layout (location = 4) in float life;

out float typeInput;
out vec3 posInput;
out vec3 velInput;
out vec3 accInput;
out float lifeInput;

void main()
{
    typeInput = type;
    posInput = pos;
    velInput = vel;
    accInput = acc;
    lifeInput = life;
}
