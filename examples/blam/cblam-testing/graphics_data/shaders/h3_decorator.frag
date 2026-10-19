#version 460 core

#extension GL_GOOGLE_include_directive : enable
/* decorators.hlsl: the set's texture by the instance's baked light, alpha
 * tested as it fades out with distance */
layout(location=0) in vec3 f_world_pos;
layout(location=1) in vec2 f_texcoord;
layout(location=2) in vec4 f_light;

layout(location=0) out vec4 out_color;

layout(location=10, binding=1) uniform sampler2D diffuse;
layout(location=11) uniform float exposure;
layout(location=12) uniform float diffuse_curve;

#include "fragments/h3_atmosphere.glsl"

void main()
{
    vec4 albedo = texture(diffuse, f_texcoord);
    if(albedo.a * f_light.a < 0.3)
        discard;
    vec3 color = pow(max(albedo.rgb, vec3(0.0)), vec3(diffuse_curve)) * f_light.rgb * exposure;
    vec3 extinction, inscatter;
    compute_scattering(camera_position, f_world_pos, extinction, inscatter);
    out_color = vec4(color * extinction + inscatter * exposure, 1.0);
}
