#version 100
precision highp float;

/* ES2 skinned mod2 (two bones per vertex). Bones arrive as three affine rows
 * each, the object transform folded in. MAX_BONES and VERTEX_XBOX come in
 * as shader constants. */
#ifndef MAX_BONES
#define MAX_BONES 41
#endif

attribute vec3 position;
attribute vec2 tex;
attribute vec2 i_node_indices;
#ifdef VERTEX_XBOX
attribute float i_node_weight0;
#else
attribute vec2 i_node_weights;
#endif

uniform mat4 camera;
uniform vec4 bones[MAX_BONES * 3];
uniform vec2 base_map_scale;

varying vec2 frag_tex;
varying vec2 frag_base_tex;

vec3 skin(vec4 p, float node)
{
    /* An unused second node may be a sentinel past the skeleton */
    int b = int(min(node, float(MAX_BONES - 1))) * 3;
    return vec3(dot(bones[b], p), dot(bones[b + 1], p), dot(bones[b + 2], p));
}

void main()
{
#ifdef VERTEX_XBOX
    /* Node indices are stored *3 and the weight out of 32767 */
    vec2  nodes   = floor(i_node_indices / 3.0 + 0.5);
    float w0      = clamp(i_node_weight0 / 32767.0, 0.0, 1.0);
    vec2  weights = vec2(w0, 1.0 - w0);
#else
    vec2 nodes   = floor(i_node_indices + 0.5);
    vec2 weights = i_node_weights;
#endif
    vec4 p     = vec4(position, 1.0);
    vec3 world = skin(p, nodes.x) * weights.x + skin(p, nodes.y) * weights.y;

    frag_tex      = tex;
    frag_base_tex = tex * base_map_scale;
    gl_Position   = camera * vec4(world, 1.0);
}
