#version 100
precision mediump float;

attribute vec3 position;
attribute vec2 tex;
attribute vec3 normal;
attribute vec3 binormal;
attribute vec3 tangent;
attribute vec2 light_tex;

uniform mat4 camera;
uniform mat3 cameraRotation;
uniform vec3 camera_position;
uniform int glw_InstanceID;

varying vec2 frag_tex;
varying vec3 frag_normal;
varying vec2 frag_light_tex;
varying float frag_instanceId;

void main()
{
    frag_tex = tex;
    frag_normal = normal;
    frag_light_tex = light_tex;
    frag_instanceId = float(glw_InstanceID);
    gl_Position = camera * vec4(position, 1.0);
}
