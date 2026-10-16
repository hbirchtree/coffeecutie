#version 460 core
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;
layout(location=3) in vec3 lightmap;
layout(location=4) in vec4 vertex_light;

layout(location=0) out vec3 f_world_pos;
layout(location=1) out vec2 f_texcoord;
layout(location=2) out vec3 f_normal;
layout(location=3) out vec2 f_detail_uv;
layout(location=4) out vec2 f_detail2_uv;
layout(location=5) out vec2 f_illum_uv;
layout(location=6) out vec3 f_lightmap;
layout(location=7) out vec2 f_mask_uv;
layout(location=8) out vec4 f_vertex_light;
layout(location=9) out vec2 f_bump_uv;

layout(location=0) uniform mat4 camera;
layout(location=1) uniform vec2 uv_scale;
/* Sky: scaled into the far plane and centred on the camera */
layout(location=4) uniform vec3 model_offset;
layout(location=5) uniform float model_scale;
layout(location=7) uniform vec2 detail_scale;
layout(location=8) uniform vec2 detail2_scale;
layout(location=12) uniform vec2 illum_scale;
layout(location=20) uniform vec2 mask_scale;
layout(location=28) uniform vec2 bump_scale;

void main()
{
    vec3 world = position * model_scale + model_offset;
    f_world_pos = world;
    f_texcoord = texcoord * uv_scale;
    f_detail_uv = texcoord * detail_scale;
    f_detail2_uv = texcoord * detail2_scale;
    f_illum_uv = texcoord * illum_scale;
    f_lightmap = lightmap;
    f_vertex_light = vertex_light;
    f_mask_uv = texcoord * mask_scale;
    f_bump_uv = texcoord * bump_scale;
    f_normal = normal;
    gl_Position = camera * vec4(world, 1.0);
}
