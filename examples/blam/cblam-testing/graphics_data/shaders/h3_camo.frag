#version 460 core

/* Active camouflage over the h3 vertex program, as camo.frag: the scene
 * behind, refracted by the view-space normal towards the silhouette */
layout(location=2) in vec3 f_normal;

layout(location=50) uniform mat3 camera_rotation; /* mat3(view) */
layout(location=53) uniform float camo_scale;
layout(location=54, binding=0) uniform sampler2D source_scene;

layout(location=0) out vec4 out_color;

const float FOUR_PI = 12.566370614;

void main()
{
    vec3  N      = normalize(camera_rotation * f_normal);
    float phase  = FOUR_PI * N.z;
    vec2  offset = vec2(N.x * sin(phase), N.y * cos(phase));
    float rim    = 1.0 - abs(N.z);
    rim *= rim;
    rim *= rim;
    vec2 uv   = gl_FragCoord.xy / vec2(textureSize(source_scene, 0));
    out_color = vec4(texture(source_scene, uv + offset * camo_scale * rim).rgb, 1.0);
}
