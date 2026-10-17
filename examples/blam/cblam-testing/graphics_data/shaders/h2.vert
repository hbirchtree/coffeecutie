#version 460 core
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;
layout(location=3) in vec3 lightmap;
layout(location=4) in vec4 vertex_light;
layout(location=5) in vec4 bone_nodes;
layout(location=6) in vec4 bone_weights;

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
/* Objects that move: object space to world */
layout(location=39) uniform mat4 model;
/* Water definitions are a polar fan (x = radius, y = angle) laid around
 * the camera at sea level: water_fan x = on, y = sea level */
layout(location=46) uniform vec2 water_fan;
layout(location=47) uniform vec2 water_eye;
/* Animated objects: bind pose to animated pose per model node */
layout(location=74) uniform int skinned;
layout(location=80) uniform mat4 bones[64]; /* 80-335 */

void main()
{
    vec4 local = vec4(position, 1.0);
    vec3 ln    = normal;
    float wsum = dot(bone_weights, vec4(1.0));
    if(skinned != 0 && wsum > 0.0)
    {
        mat4 skin = (bones[int(bone_nodes.x)] * bone_weights.x +
                     bones[int(bone_nodes.y)] * bone_weights.y +
                     bones[int(bone_nodes.z)] * bone_weights.z +
                     bones[int(bone_nodes.w)] * bone_weights.w) / wsum;
        local = skin * local;
        ln    = mat3(skin) * ln;
    }
    vec3 world = (model * local).xyz * model_scale + model_offset;
    vec2 uv    = texcoord;
    vec3 n     = mat3(model) * ln;
    if(water_fan.x > 0.5)
    {
        float angle = position.y / 418.546 * 3.14159265 * 1.03;
        vec2  xy    = water_eye + position.x * vec2(cos(angle), sin(angle));
        world        = vec3(xy, water_fan.y);
        uv           = xy;
        n            = vec3(0.0, 0.0, 1.0);
    }
    f_world_pos = world;
    f_texcoord = uv * uv_scale;
    f_detail_uv = texcoord * detail_scale;
    f_detail2_uv = texcoord * detail2_scale;
    f_illum_uv = texcoord * illum_scale;
    f_lightmap = lightmap;
    f_vertex_light = vertex_light;
    f_mask_uv = texcoord * mask_scale;
    f_bump_uv = texcoord * bump_scale;
    f_normal = n;
    gl_Position = camera * vec4(world, 1.0);
}
