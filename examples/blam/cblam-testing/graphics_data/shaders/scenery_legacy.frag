#version 100
precision mediump float;

/* ES2 legacy scenery fragment shader: single diffuse map, no lightmap /
 * multipurpose / reflection (experiment). */
/* Scaled UVs reach the hundreds; mediump is fp16 on SGX/Mali-4xx */
#ifdef GL_FRAGMENT_PRECISION_HIGH
#define TEXCOORD_PRECISION highp
#else
#define TEXCOORD_PRECISION mediump
#endif

varying TEXCOORD_PRECISION vec2 frag_base_tex;

uniform sampler2D diffuse;

void main()
{
    gl_FragColor = texture2D(diffuse, frag_base_tex);
}
