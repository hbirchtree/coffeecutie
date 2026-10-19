#version 100
precision highp float;

/* Enhanced ES2 materials. What varies per material arrives as shader
 * constants:
 *   GEOMETRY     0 BSP, 1 batched models, 2 skinned models
 *   MATERIAL     materials::id: 1 senv, 4/5 chicago, 7 sotr, 9 soso
 *   LIGHTMAP     models: the batch samples the lightmap under each object
 * Lighting and reflection vectors are worked out here, per vertex. */
#ifndef GEOMETRY
#define GEOMETRY 0
#endif
#ifndef MATERIAL
#define MATERIAL 1
#endif
#ifndef LIGHTMAP
#define LIGHTMAP 0
#endif
#ifndef MODEL_BATCH
#define MODEL_BATCH 16
#endif
#ifndef MAX_BONES
#define MAX_BONES 32
#endif
#ifndef SKY_LIGHTS
#define SKY_LIGHTS 4
#endif

attribute vec3 position;
attribute vec2 tex;
#if GEOMETRY == 0
attribute vec3 normal;
attribute vec3 binormal;
attribute vec3 tangent;
attribute vec2 light_tex;
#else
attribute vec3 i_normal;
#endif
#if GEOMETRY == 2
attribute vec2 i_node_indices;
#ifdef VERTEX_XBOX
attribute float i_node_weight0;
#else
attribute vec2 i_node_weights;
#endif
#endif

uniform mat4 camera;
uniform vec3 eye;
uniform vec4 map_uv[5]; /* scale, scroll offset */
uniform vec3 key_light;  /* senv: the sun, for bump shading */

#if GEOMETRY == 1
/* Per instance: three affine rows, lightmap uv, light incident direction
 * (length = how directional) */
uniform vec4 instances[MODEL_BATCH * 5];
uniform int  glw_InstanceID;
#elif GEOMETRY == 2
uniform vec4 bones[MAX_BONES * 3];
uniform vec4 probe[2];
#endif

/* Lit materials on objects take their light per vertex */
#if GEOMETRY != 0 && (MATERIAL == 1 || MATERIAL == 9)
#define OBJECT_LIGHTING 1
#else
#define OBJECT_LIGHTING 0
#endif

#if OBJECT_LIGHTING && LIGHTMAP == 0
/* Objects with no lightmap below: ambient plus the sky's lights */
uniform vec3 ambient;
uniform vec4 sky_lights[SKY_LIGHTS * 2]; /* direction, color * power */
#endif

varying vec4 frag_uv01;
varying vec4 frag_uv23;
varying vec4 frag_uv4l;    /* map 4, lightmap */
varying vec4 frag_light;   /* senv: light in tangent space; soso: light */
varying vec4 frag_reflect; /* cube direction, 1 - N.V */

vec2 map_coord(int i)
{
    return tex * map_uv[i].xy + map_uv[i].zw;
}

#if GEOMETRY == 2
vec4 bone_row(int b, int r)
{
    return bones[b * 3 + r];
}

void skin(float node, float weight, vec4 p, vec4 n, inout vec3 wp, inout vec3 wn)
{
    /* An unused second node may be a sentinel past the skeleton */
    int b = int(min(node, float(MAX_BONES - 1)));
    vec4 r0 = bone_row(b, 0);
    vec4 r1 = bone_row(b, 1);
    vec4 r2 = bone_row(b, 2);
    wp += vec3(dot(r0, p), dot(r1, p), dot(r2, p)) * weight;
    wn += vec3(dot(r0, n), dot(r1, n), dot(r2, n)) * weight;
}
#endif

void main()
{
    vec4 p = vec4(position, 1.0);
#if GEOMETRY == 0
    vec3 world = position;
    vec3 n     = normal;
    vec2 light_uv = light_tex;
#elif GEOMETRY == 1
    int  base  = glw_InstanceID * 5;
    vec4 nn    = vec4(i_normal, 0.0);
    vec3 world = vec3(
        dot(instances[base], p),
        dot(instances[base + 1], p),
        dot(instances[base + 2], p));
    vec3 n = vec3(
        dot(instances[base], nn),
        dot(instances[base + 1], nn),
        dot(instances[base + 2], nn));
    vec2 light_uv  = instances[base + 3].xy;
    vec3 light_dir = instances[base + 4].xyz;
#else
#ifdef VERTEX_XBOX
    /* Node indices are stored *3 and the weight out of 32767 */
    vec2  nodes   = floor(i_node_indices / 3.0 + 0.5);
    float w0      = clamp(i_node_weight0 / 32767.0, 0.0, 1.0);
    vec2  weights = vec2(w0, 1.0 - w0);
#else
    vec2 nodes   = floor(i_node_indices + 0.5);
    vec2 weights = i_node_weights;
#endif
    vec3 world = vec3(0.0);
    vec3 n     = vec3(0.0);
    skin(nodes.x, weights.x, p, vec4(i_normal, 0.0), world, n);
    skin(nodes.y, weights.y, p, vec4(i_normal, 0.0), world, n);
    vec2 light_uv  = probe[0].xy;
    vec3 light_dir = probe[1].xyz;
#endif
    n = normalize(n);

    frag_uv01 = vec4(map_coord(0), map_coord(1));
    frag_uv23 = vec4(map_coord(2), map_coord(3));
    frag_uv4l = vec4(map_coord(4), light_uv);

    vec3 view = normalize(eye - world);
    vec3 r    = reflect(-view, n);
    /* Cube maps are Y-up and left-handed */
    frag_reflect = vec4(-r.y, r.z, r.x, 1.0 - clamp(dot(n, view), 0.0, 1.0));

#if GEOMETRY == 0
    frag_light = vec4(
        dot(key_light, tangent), dot(key_light, binormal), dot(key_light, n), 1.0);
#elif OBJECT_LIGHTING && LIGHTMAP == 1
    /* Brighter on the side facing the radiosity, by how directional it is */
    float strength = length(light_dir);
    float facing   = strength > 0.0001
        ? max(0.0, dot(n, light_dir / strength)) : 1.0;
    frag_light = vec4(vec3(mix(1.0, 2.0 * facing, min(strength, 1.0))), 1.0);
#elif OBJECT_LIGHTING
    vec3 lit = ambient;
    for(int i = 0; i < SKY_LIGHTS; i++)
        lit += sky_lights[i * 2 + 1].rgb
             * max(0.0, dot(n, sky_lights[i * 2].xyz));
    frag_light = vec4(clamp(lit, 0.1, 1.0), 1.0);
#else
    frag_light = vec4(1.0);
#endif

    gl_Position = camera * vec4(world, 1.0);
}
