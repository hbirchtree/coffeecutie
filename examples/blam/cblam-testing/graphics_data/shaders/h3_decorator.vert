#version 460 core
/* decorators.hlsl: one mesh drawn at every instance of a decorator set */
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;

layout(location=0) out vec3 f_world_pos;
layout(location=1) out vec2 f_texcoord;
layout(location=2) out vec4 f_light; /* baked light, fade */

layout(location=0) uniform mat4 camera;
layout(location=1) uniform vec3 eye;
/* Where the set starts and finishes fading out with distance */
layout(location=2) uniform vec2 fade;
layout(location=3) uniform int first_instance;
/* Three texels an instance: position, rotation (its length squared the
 * scale), baked light */
layout(location=4, binding=0) uniform sampler2D instances;

vec4 instance_texel(int i)
{
    int width = textureSize(instances, 0).x;
    return texelFetch(instances, ivec2(i % width, i / width), 0);
}

void main()
{
    int  i        = (first_instance + gl_InstanceID) * 3;
    vec3 origin   = instance_texel(i).xyz;
    vec4 q        = instance_texel(i + 1);
    vec3 light    = instance_texel(i + 2).rgb;
    float scale   = dot(q, q);
    vec3 axis     = q.xyz / max(sqrt(scale), 1e-6);
    float w       = q.w / max(sqrt(scale), 1e-6);
    vec3 rotated  = position + 2.0 * cross(axis, cross(axis, position) + w * position);
    vec3 world    = rotated * scale + origin;
    float distance = length(world - eye);
    f_world_pos   = world;
    f_texcoord    = texcoord;
    f_light       = vec4(light, clamp((fade.y - distance) / max(fade.y - fade.x, 1e-3), 0.0, 1.0));
    gl_Position   = camera * vec4(world, 1.0);
}
