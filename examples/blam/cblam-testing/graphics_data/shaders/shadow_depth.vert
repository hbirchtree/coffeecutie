#version 460 core
/* Shadow casters: position only, whatever else the vertex format carries */
layout(location=0) in vec3 position;

layout(location=0) uniform mat4 light;
layout(location=4) uniform mat4 model;

void main()
{
    gl_Position = light * model * vec4(position, 1.0);
}
