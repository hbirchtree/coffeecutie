#version 100
precision highp float;

/* ES2 counterpart of ui.vert: each quad's instance data comes per vertex */
attribute vec2 position;
attribute vec2 tex_coord;
attribute vec4 color;
attribute vec4 tex_scale_offset;
attribute vec4 params; /* flags, meter fill */
attribute vec4 empty;  /* meters: the unlit color */

uniform mat3 screen_scale;

varying vec2 frag_tex;
varying vec4 frag_color;
varying vec4 frag_params;
varying vec4 frag_empty;

void main()
{
    gl_Position = vec4((screen_scale * vec3(position, 1.0)).xy, 0.0, 1.0);
    frag_tex    = tex_coord * tex_scale_offset.xy + tex_scale_offset.zw;
    frag_color  = color;
    frag_params = params;
    frag_empty  = empty;
}
