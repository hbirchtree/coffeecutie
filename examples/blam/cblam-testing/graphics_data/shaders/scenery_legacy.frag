#version 100
precision mediump float;

/* ES2 legacy scenery fragment shader: single diffuse map, no lightmap /
 * multipurpose / reflection (experiment). */
varying vec2 frag_tex;

uniform sampler2D diffuse;
uniform vec2      base_map_scale;

void main()
{
    gl_FragColor = texture2D(diffuse, frag_tex * base_map_scale);
}
