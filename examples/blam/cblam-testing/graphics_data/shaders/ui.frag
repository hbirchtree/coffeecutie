#version 460 core

layout(location = 0) in FragmentData {
    vec2 tex_coord;
    flat int element_id;
} frag;

struct Instance
{
    vec4 color;
    vec4 tex_scale_offset;
    uvec4 texture_source; // x = the value
    uvec4 extra; // x = flags; meters: y = empty color (RGBA8), z = fill
};

/* Lowered at load where uniform blocks are small (materials::ui_instances) */
layout(constant_id = 1) const int UI_INSTANCES = 512;

layout(binding = 0, std140) uniform InstanceData {
    Instance instances[UI_INSTANCES];
} elements;

layout(location = 10, binding = 0) uniform sampler2DArray source_bc1;
layout(location = 11, binding = 1) uniform sampler2DArray source_bc2;
layout(location = 12, binding = 2) uniform sampler2DArray source_bc3;
layout(location = 13, binding = 3) uniform sampler2DArray source_rgba4;
layout(location = 14, binding = 4) uniform sampler2DArray source_rgba8;
layout(location = 15, binding = 5) uniform sampler2DArray source_font;
layout(location = 16, binding = 6) uniform sampler2DArray source_rg8;
layout(location = 17, binding = 7) uniform sampler2DArray source_etc2_rgb;
layout(location = 18, binding = 8) uniform sampler2DArray source_etc2_rgba;
layout(location = 19, binding = 9) uniform sampler2DArray source_r8;
layout(location = 20, binding = 10) uniform sampler2DArray source_rgb565;

const uint TEX_BC1    = 1u;
const uint TEX_BC2    = 2u;
const uint TEX_BC3    = 3u;
const uint TEX_RGB565 = 4u;
const uint TEX_R8     = 5u;
const uint TEX_RG8    = 6u;
const uint TEX_RGBA4  = 7u;
const uint TEX_RGBA8  = 8u;
/* Xbox luminance/alpha formats, kept in the R8 bucket */
const uint TEX_A8     = 9u;  /* 111A */
const uint TEX_Y8     = 10u; /* LLL1 */
const uint TEX_AY8    = 11u; /* LLLL */
/* Past the bitmap cache's codes */
const uint TEX_FONT   = 15u;
const uint TEX_A8Y8   = 12u;
const uint TEX_ETC2_RGB  = 13u;
const uint TEX_ETC2_RGBA = 14u;

const uint USE_ALPHA = 1u;
const uint METER     = 2u;
const uint SOLID     = 4u;
const uint PREMULTIPLIED = 8u;

vec4 unpack_rgba8(uint v)
{
    return vec4(v & 0xFFu, (v >> 8) & 0xFFu, (v >> 16) & 0xFFu, v >> 24) / 255.0;
}

/* Half a texel inside the sprite's rectangle, so filtering never reaches
 * whatever is packed next to it */
vec2 inside(sampler2DArray s, vec2 uv, vec4 scale_offset)
{
    vec2 half_texel = 0.5 / vec2(textureSize(s, 0).xy);
    vec2 a = scale_offset.zw;
    vec2 b = scale_offset.zw + scale_offset.xy;
    return clamp(uv, min(a, b) + half_texel, max(a, b) - half_texel);
}

vec4 sample_color()
{
    vec4 scale_offset = elements.instances[frag.element_id].tex_scale_offset;
    vec2 sample_coord = frag.tex_coord * scale_offset.xy + scale_offset.zw;
    uint texture_source = elements.instances[frag.element_id].texture_source.x;
    uint source = texture_source >> 24;
    uint layer = texture_source & 0xFFFFFF;
    const float bias = -10.0;
    if(source == TEX_BC1)
        return texture(source_bc1, vec3(inside(source_bc1, sample_coord, scale_offset), layer), bias);
    else if(source == TEX_BC2)
        return texture(source_bc2, vec3(inside(source_bc2, sample_coord, scale_offset), layer), bias);
    else if(source == TEX_BC3)
        return texture(source_bc3, vec3(inside(source_bc3, sample_coord, scale_offset), layer), bias);
    /* RGBA4 = D3DFMT_A4R4G4B4: GL reads (A,R,G,B) in bits[15:12..3:0],
     * giving texture.rgba=(Halo.A, Halo.R, Halo.G, Halo.B).
     * .gbar reorders to (Halo.R, Halo.G, Halo.B, Halo.A). */
    else if(source == TEX_RGBA4)
        return texture(source_rgba4, vec3(inside(source_rgba4, sample_coord, scale_offset), layer), bias).gbar;
    /* RGBA8 = D3DFMT_A8R8G8B8: bytes in memory are B,G,R,A (little-endian),
     * giving texture.rgba=(B, G, R, A). .bgra reorders to (R, G, B, A). */
    else if(source == TEX_RGBA8)
        return texture(source_rgba8, vec3(inside(source_rgba8, sample_coord, scale_offset), layer), bias).bgra;
    else if(source == TEX_RGB565)
        return texture(source_rgb565, vec3(inside(source_rgb565, sample_coord, scale_offset), layer), bias).bgra;
    else if(source == TEX_R8)
        return texture(source_r8, vec3(inside(source_r8, sample_coord, scale_offset), layer), bias);
    else if(source == TEX_RG8)
        return texture(source_rg8, vec3(inside(source_rg8, sample_coord, scale_offset), layer), bias);
    else if(source == TEX_A8)
        return vec4(1.0, 1.0, 1.0, texture(source_r8, vec3(inside(source_r8, sample_coord, scale_offset), layer), bias).r);
    else if(source == TEX_Y8)
        return vec4(vec3(texture(source_r8, vec3(inside(source_r8, sample_coord, scale_offset), layer), bias).r), 1.0);
    else if(source == TEX_AY8)
        return vec4(texture(source_r8, vec3(inside(source_r8, sample_coord, scale_offset), layer), bias).rrrr);
    /* A8Y8: luminance in r, alpha in g; tinted by the instance colour */
    else if(source == TEX_A8Y8)
    {
        vec2 la = texture(source_rg8, vec3(inside(source_rg8, sample_coord, scale_offset), layer), bias).rg;
        return vec4(la.rrr, la.g);
    }
    else if(source == TEX_ETC2_RGB)
        return texture(source_etc2_rgb, vec3(inside(source_etc2_rgb, sample_coord, scale_offset), layer), bias);
    else if(source == TEX_ETC2_RGBA)
        return texture(source_etc2_rgba, vec3(inside(source_etc2_rgba, sample_coord, scale_offset), layer), bias);
    else if(source == TEX_FONT)
    {
        float alpha = texture(source_font, vec3(inside(source_font, sample_coord, scale_offset), layer)).r;
        vec4 col = elements.instances[frag.element_id].color;
        return vec4(col.rgb, alpha * col.a);
    }
    else
        return vec4(1.0, 0.0, 1.0, 0.0);
}

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 coloring = elements.instances[frag.element_id].color.rgb;
    vec4 background_color = sample_color();
    uint source = elements.instances[frag.element_id].texture_source.x >> 24;
    uvec4 extra = elements.instances[frag.element_id].extra;
    vec4 color = elements.instances[frag.element_id].color;
    if((extra.x & SOLID) != 0u)
        out_color = color;
    else if(source == TEX_FONT)
        out_color = background_color;
    /* Lit where the texel's alpha ramp is under the fill; the shape is in
     * its color channels */
    else if((extra.x & METER) != 0u)
    {
        bool lit = background_color.a <= uintBitsToFloat(extra.z);
        vec4 tint = lit ? color : unpack_rgba8(extra.y);
        float shape = max(background_color.r, max(background_color.g, background_color.b));
        out_color = vec4(tint.rgb, tint.a) * shape;
    }
    /* HUD: bitmaps carry their coverage in the color channels, and a
     * color's alpha is how much of what is behind it hides; none adds on */
    else if((extra.x & PREMULTIPLIED) != 0u)
    {
        /* A8 keeps its coverage in alpha alone */
        vec3 cover = source == TEX_A8 ? vec3(background_color.a) : background_color.rgb;
        out_color = vec4(color.rgb * cover, color.a * background_color.a);
    }
    else if((extra.x & USE_ALPHA) != 0u)
        out_color = vec4(color.rgb * background_color.rgb, background_color.a * color.a);
    else
        out_color = vec4(coloring.rgb * background_color.rgb, background_color.a);
}
