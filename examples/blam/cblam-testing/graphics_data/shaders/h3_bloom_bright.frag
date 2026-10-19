#version 460 core

/* What blooms, at a quarter of the scene's size: downsample_4x4_block_bloom.hlsl */
layout(location=0) in vec2 f_uv;

layout(location=1, binding=0) uniform sampler2D scene_color;
layout(location=2) uniform vec2 source_texel;
/* Bloom point, inherent bloom */
layout(location=3) uniform vec2 bloom_curve;

layout(location=0) out vec4 out_color;

void main()
{
    vec3 color = vec3(1e-8);
    color += texture(scene_color, f_uv + source_texel * vec2(-1.0, -1.0)).rgb;
    color += texture(scene_color, f_uv + source_texel * vec2(1.0, -1.0)).rgb;
    color += texture(scene_color, f_uv + source_texel * vec2(-1.0, 1.0)).rgb;
    color += texture(scene_color, f_uv + source_texel * vec2(1.0, 1.0)).rgb;
    color *= 0.25;
    float intensity = dot(color, vec3(0.2126, 0.7152, 0.0722));
    float curved    = max(intensity * bloom_curve.y, intensity - bloom_curve.x);
    out_color       = vec4(color * (max(curved, 0.0) / max(intensity, 1e-8)), 1.0);
}
