#version 460 core

/* The scene before water: its colour, and the distance from the camera to
 * what each pixel shows, for refraction and water depth */
layout(location=0) in vec2 f_uv;

layout(location=1, binding=0) uniform sampler2D scene_color;
layout(location=2, binding=1) uniform sampler2D scene_depth;
layout(location=3) uniform mat4 inverse_camera;
layout(location=4) uniform vec3 eye;
/* 1: depth is clip-control 0..1, 0: OpenGL's -1..1 */
layout(location=5) uniform int depth_zero_to_one;

layout(location=0) out vec4 out_color;

void main()
{
    float depth = texture(scene_depth, f_uv).r;
    float z     = depth_zero_to_one != 0 ? depth : depth * 2.0 - 1.0;
    vec4  world = inverse_camera * vec4(f_uv * 2.0 - 1.0, z, 1.0);
    float dist  = abs(world.w) > 1e-8 ? length(world.xyz / world.w - eye) : 1e6;
    out_color   = vec4(texture(scene_color, f_uv).rgb, min(dist, 1e6));
}
