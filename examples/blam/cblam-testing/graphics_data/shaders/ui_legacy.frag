#version 100
precision mediump float;

/* ES2 counterpart of ui.frag: one texture per draw, its format a uniform */
#ifdef GL_FRAGMENT_PRECISION_HIGH
#define TEXCOORD_PRECISION highp
#else
#define TEXCOORD_PRECISION mediump
#endif

varying TEXCOORD_PRECISION vec2 frag_tex;
varying vec4 frag_color;
varying vec4 frag_params;
varying vec4 frag_empty;

uniform sampler2D source;
uniform float     format;      /* texture_source >> 24 */
uniform float     la_textures; /* 1: two-channel images are luminance-alpha */

const float TEX_RGBA4 = 7.0;
const float TEX_RGBA8 = 8.0;
const float TEX_FONT  = 9.0;
const float TEX_A8Y8  = 12.0;

bool flag(float bit)
{
    return mod(floor(frag_params.x / bit), 2.0) > 0.5;
}

vec4 sample_color()
{
    if(format < 0.5)
        return vec4(1.0, 0.0, 1.0, 0.0);
    vec4 t = texture2D(source, frag_tex);
    /* A4R4G4B4 reads as (A, R, G, B), A8R8G8B8 bytes are B, G, R, A */
    if(abs(format - TEX_RGBA4) < 0.5)
        return t.gbar;
    if(abs(format - TEX_RGBA8) < 0.5)
        return t.bgra;
    if(abs(format - TEX_A8Y8) < 0.5)
        return vec4(t.rrr, mix(t.g, t.a, la_textures));
    if(abs(format - TEX_FONT) < 0.5)
        return vec4(frag_color.rgb, t.r * frag_color.a);
    return t;
}

void main()
{
    vec4 background = sample_color();
    vec4 color      = frag_color;
    if(flag(4.0))
        gl_FragColor = color;
    else if(abs(format - TEX_FONT) < 0.5)
        gl_FragColor = background;
    /* Lit where the texel's alpha ramp is under the fill; the shape is in
     * its color channels */
    else if(flag(2.0))
    {
        vec4  tint  = background.a <= frag_params.y ? color : frag_empty;
        float shape = max(background.r, max(background.g, background.b));
        gl_FragColor = vec4(tint.rgb, shape * tint.a);
    }
    else if(flag(1.0))
        gl_FragColor = vec4(color.rgb * background.rgb, background.a * color.a);
    else
        gl_FragColor = vec4(color.rgb * background.rgb, background.a);
}
