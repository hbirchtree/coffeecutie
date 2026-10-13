#version 100
precision mediump float;

/* ES2 counterpart of effects.frag: one texture per draw, its format as a uniform */
#ifdef GL_FRAGMENT_PRECISION_HIGH
#define TEXCOORD_PRECISION highp
#else
#define TEXCOORD_PRECISION mediump
#endif

varying TEXCOORD_PRECISION vec4 frag_tex; /* uv, repeats, opaque */
varying vec4 frag_color;
varying TEXCOORD_PRECISION vec4 frag_atlas;

uniform sampler2D source;
uniform float     format;      /* BitmapCache::type_mask code */
uniform float     blend_mode;  /* 0 alpha, 1 add, 2 multiply, 3 double multiply */
uniform float     la_textures; /* 1: two-channel images are luminance-alpha */
uniform vec2      texel_count;

vec4 sample_texture()
{
    TEXCOORD_PRECISION vec2 uv = frag_tex.z > 0.5 ? fract(frag_tex.xy) : frag_tex.xy;
    uv = uv * frag_atlas.xy + frag_atlas.zw;
    if(format < 0.5)
    {
        float r = length(frag_tex.xy * 2.0 - 1.0);
        float a = clamp(1.0 - r, 0.0, 1.0);
        return vec4(1.0, 1.0, 1.0, a * a);
    }
    if(format > 14.5)
    {
        /* D3D A1R5G5B5 words read as GL 5_5_5_1: shift the bits back */
        vec2 texel = (floor(uv * texel_count) + 0.5) / texel_count;
        vec4 t     = floor(texture2D(source, texel) * 31.0 + 0.5);
        return vec4(
                   mod(t.r, 16.0) * 2.0 + floor(t.g / 16.0),
                   mod(t.g, 16.0) * 2.0 + floor(t.b / 16.0),
                   mod(t.b, 16.0) * 2.0 + step(15.5, t.a),
                   31.0 * floor(t.r / 16.0)) /
               31.0;
    }
    vec4  t      = texture2D(source, uv);
    float second = mix(t.g, t.a, la_textures);
    if(format < 3.5)
        return t;
    if(format < 4.5)
        return vec4(t.rgb, 1.0);
    if(format < 5.5)
        return t.rrrr;
    if(format < 6.5)
        return vec4(t.rrr, second);
    /* A4R4G4B4 reads as (A, R, G, B) */
    if(format < 7.5)
        return t.gbar;
    /* A8R8G8B8 bytes are B, G, R, A */
    if(format < 8.5)
        return t.bgra;
    if(format < 9.5)
        return vec4(1.0, 1.0, 1.0, t.r);
    if(format < 10.5)
        return vec4(t.rrr, 1.0);
    if(format < 11.5)
        return t.rrrr;
    if(format < 12.5)
        return vec4(t.rrr, second);
    return t;
}

void main()
{
    vec4 c = sample_texture();
    if(frag_tex.w > 0.5)
        c.a = 1.0;
    c *= frag_color;
    if(blend_mode > 2.5)
        gl_FragColor = vec4(mix(vec3(1.0), 2.0 * c.rgb, c.a), 1.0);
    else if(blend_mode > 1.5)
        gl_FragColor = vec4(mix(vec3(1.0), c.rgb, c.a), 1.0);
    else
        gl_FragColor = c;
}
