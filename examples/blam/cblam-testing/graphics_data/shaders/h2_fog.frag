#version 460 core
/* Planar fog over the background, taken to be `sky_distance` away: no
 * geometry was drawn there to fog itself */
layout(location=0) in vec2 f_ndc;

layout(location=0) out vec4 out_color;

layout(location=0) uniform mat4 inverse_camera; /* 0-3 */
layout(location=4) uniform vec3 fog_eye;
layout(location=5) uniform vec4 planar_fog_plane;
layout(location=6) uniform vec4 planar_fog_color;
layout(location=7) uniform vec2 planar_fog_opaque;

const float sky_distance = 500.0;

void main()
{
    vec4 p   = inverse_camera * vec4(f_ndc, 0.5, 1.0);
    vec3 dir = normalize(p.xyz / p.w - fog_eye);
    /* Same integral as fogged(), from the eye to the background */
    float de = planar_fog_plane.w - dot(planar_fog_plane.xyz, fog_eye);
    float dp = de - dot(planar_fog_plane.xyz, dir) * sky_distance;
    float t0 = de > 0.0 ? 0.0 : (dp > 0.0 ? de / (de - dp) : 1.0);
    float t1 = dp > 0.0 ? 1.0 : (de > 0.0 ? de / (de - dp) : 0.0);
    float len = max(t1 - t0, 0.0) * sky_distance;
    float depth0 = max(mix(de, dp, t0), 0.0);
    float depth1 = max(mix(de, dp, t1), 0.0);
    float opaque_depth = max(planar_fog_opaque.y, 1e-3);
    float density = 0.5 * (clamp(depth0 / opaque_depth, 0.0, 1.0) +
                           clamp(depth1 / opaque_depth, 0.0, 1.0));
    float fog = planar_fog_opaque.x > 0.0
        ? 1.0 - exp(-density * len / planar_fog_opaque.x)
        : clamp(max(depth0, depth1) / opaque_depth, 0.0, 1.0);
    out_color = vec4(planar_fog_color.rgb, min(fog, planar_fog_color.a));
}
