#version 460 core

/* Active camouflage: screen-space refraction of the already-resolved scene,
 * driven by the surface normal in view space.
 *
 * This is the Xbox active_camouflage_distortion cube map evaluated directly.
 * That map is a lookup table, not an environment map -- it holds no scene
 * colour, only the function
 *
 *     offset.x = N.x * sin(4*pi * N.z)
 *     offset.y = N.y * cos(4*pi * N.z)
 *     weight   = pow(1 - abs(N.z), 4)
 *
 * for a unit N in view space, reproduced here to under one 8-bit LSB. It
 * existed because the NV2A had no per-pixel ALU; a sincos and two muls beat
 * the dependent cube fetch and drop the table's 32x32 stair-stepping.
 *
 * sin on x and cos on y put the two axes a quarter cycle apart, so the smear
 * alternates horizontal and vertical across the silhouette. The phase runs two
 * full cycles from face-on to edge-on, which is what gives the banded, doubled
 * edge rather than a smooth bulge.
 *
 * N must be in view space against the camera axis, not against a per-fragment
 * view vector -- the two disagree by up to 0.4 off-axis, enough to flip the
 * sign of n_z and mirror the bands.
 *
 * Must run in a pass after the scene resolve: source_scene cannot alias the
 * render target being written.
 */

/* Mirrors the output block in fragments/scenery_common.glsl member for member;
 * interface blocks link by layout, not by use. Only normal is read here. */
layout(location = 0) in FragData {
    vec3 position;
    vec2 tex;
    vec3 tangent;
    vec3 binormal;
    vec3 normal;
    vec2 light_tex;
    flat int instanceId;
} frag;

layout(location = 2)  uniform mat3  cameraRotation; /* mat3(view) */
layout(location = 24) uniform float camo_scale;     /* refraction strength, UV */

layout(location = 23, binding = 12) uniform sampler2D source_scene;

layout(location = 0) out vec4 final_color;

const float FOUR_PI = 12.566370614;

void main()
{
    vec3 N = normalize(cameraRotation * frag.normal);

    float phase  = FOUR_PI * N.z;
    vec2  offset = vec2(N.x * sin(phase), N.y * cos(phase));

    /* Fresnel rim: zero facing the camera, peak at the silhouette. pow(x, 4)
     * by squaring twice. */
    float rim = 1.0 - abs(N.z);
    rim *= rim;
    rim *= rim;

    /* Offset stays in eye-space units, as on Xbox, so the smear is slightly
     * wider than tall on a non-square viewport. Divide the x term by the
     * aspect ratio if a circular smear is wanted instead. */
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(source_scene, 0));
    final_color = vec4(texture(source_scene, uv + offset * camo_scale * rim).rgb, 1.0);
}
