#version 460 core
#extension GL_GOOGLE_include_directive : require

void vertex_main()
{
    gl_Position = vec4(1.0, 0.0, 1.0, 1.0);
}

layout(location = 0) out vec4 final_color;

void fragment_main()
{
    final_color = vec4(vec3(1.0, 0.0, 1.0), 1.0);
}
