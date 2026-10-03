#version 100

precision mediump float;

varying vec3 in_normal;
varying vec3 in_view_dir;

uniform vec3 color;

void main()
{
    /* Headlight: lit from the viewer, so it reads the same in any space */
    float diffuse =
        max(dot(normalize(in_normal), normalize(in_view_dir)), 0.0);
    gl_FragColor = vec4(color * (0.25 + 0.75 * diffuse), 1.0);
}
