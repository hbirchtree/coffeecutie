#version 100
precision mediump float;

/* ES2 legacy scenery (mod2) vertex shader: diffuse only, no skinning.
 * Per-object transform passed as a plain uniform (no MatrixStore UBO). */
attribute vec3 position;
attribute vec2 tex;

uniform mat4 camera; /* view-projection (already folds the BSP basis) */
uniform mat4 model;  /* per-object world transform */

varying vec2 frag_tex;

void main()
{
    frag_tex    = tex;
    gl_Position = camera * model * vec4(position, 1.0);
}
