#version 460 core

layout(location = 0) in FragmentData
{
    vec2      tex;
    vec4      color;
    flat vec4 atlas;
    flat vec4 source;
} frag;

layout(location = 4) uniform float blend_mode; /* 0 alpha, 1 add, 2 multiply */

layout(location = 10, binding = 0) uniform sampler2DArray source_bc1;
layout(location = 11, binding = 1) uniform sampler2DArray source_bc2;
layout(location = 12, binding = 2) uniform sampler2DArray source_bc3;
layout(location = 13, binding = 3) uniform sampler2DArray source_rgb565;
layout(location = 14, binding = 4) uniform sampler2DArray source_r8;
layout(location = 15, binding = 5) uniform sampler2DArray source_rg8;
layout(location = 16, binding = 6) uniform sampler2DArray source_rgba4;
layout(location = 17, binding = 7) uniform sampler2DArray source_rgba8;
layout(location = 18, binding = 8) uniform sampler2DArray source_etc2_rgb;
layout(location = 19, binding = 9) uniform sampler2DArray source_etc2_rgba;
layout(location = 20, binding = 10) uniform sampler2DArray source_rgb5a1;

layout(location = 0) out vec4 out_color;

vec4 sample_texture()
{
    uint  format = uint(frag.source.x + 0.5);
    float layer  = frag.source.y;
    /* Ribbons repeat along their length within the atlas tile */
    vec2 uv     = frag.source.z > 0.5 && frag.source.z < 1.5 ? fract(frag.tex) : frag.tex;
    vec3 coord  = vec3(uv * frag.atlas.xy + frag.atlas.zw, layer);
    if(format == 0u)
    {
        /* No bitmap: a soft round glow */
        float r = length(frag.tex * 2.0 - 1.0);
        float a = clamp(1.0 - r, 0.0, 1.0);
        return vec4(1.0, 1.0, 1.0, a * a);
    }
    if(format == 1u)
        return texture(source_bc1, coord);
    if(format == 2u)
        return texture(source_bc2, coord);
    if(format == 3u)
        return texture(source_bc3, coord);
    if(format == 4u)
        return vec4(texture(source_rgb565, coord).rgb, 1.0);
    /* Luminance/alpha formats upload as one channel */
    if(format == 9u)
        return vec4(1.0, 1.0, 1.0, texture(source_r8, coord).r);
    if(format == 10u)
        return vec4(texture(source_r8, coord).rrr, 1.0);
    if(format == 11u || format == 5u)
        return texture(source_r8, coord).rrrr;
    if(format == 12u)
    {
        vec2 la = texture(source_rg8, coord).rg;
        return vec4(la.rrr, la.g);
    }
    if(format == 6u)
        return texture(source_rg8, coord).rrrg;
    /* A4R4G4B4 reads as (A, R, G, B) */
    if(format == 7u)
        return texture(source_rgba4, coord).gbar;
    /* A8R8G8B8 bytes are B, G, R, A */
    if(format == 8u)
        return texture(source_rgba8, coord).bgra;
    if(format == 13u)
        return texture(source_etc2_rgb, coord);
    if(format == 14u)
        return texture(source_etc2_rgba, coord);
    if(format == 15u)
    {
        /* D3D A1R5G5B5 words, uploaded as GL 5_5_5_1: rebuild the word */
        ivec3 size  = textureSize(source_rgb5a1, 0);
        ivec2 texel = clamp(ivec2(coord.xy * vec2(size.xy)), ivec2(0), size.xy - 1);
        vec4  t     = texelFetch(source_rgb5a1, ivec3(texel, int(layer)), 0);
        uint  word  = (uint(t.r * 31.0 + 0.5) << 11) | (uint(t.g * 31.0 + 0.5) << 6) |
                     (uint(t.b * 31.0 + 0.5) << 1) | (t.a > 0.5 ? 1u : 0u);
        return vec4(
            float((word >> 10) & 31u) / 31.0,
            float((word >> 5) & 31u) / 31.0,
            float(word & 31u) / 31.0,
            float(word >> 15));
    }
    return vec4(1.0, 0.0, 1.0, 1.0);
}

void main()
{
    vec4 c = sample_texture();
    if(frag.source.w > 0.5)
        c.a = 1.0;
    c *= frag.color;
    if(blend_mode > 1.5)
        out_color = vec4(mix(vec3(1.0), c.rgb, c.a), 1.0);
    else
        out_color = c;
}
