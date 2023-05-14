#version 100

precision mediump float;

attribute vec3 pos;
attribute vec2 tex;

uniform mat4 transform[8];

uniform int g_InstanceID;

varying vec2 vs_tc;
varying float fInstanceID;

void main(void)
{
    /* Regular vertex properties */
    vs_tc = tex;
    fInstanceID = float(g_InstanceID);
    gl_Position = transform[g_InstanceID] * vec4(pos, 1.);
}
