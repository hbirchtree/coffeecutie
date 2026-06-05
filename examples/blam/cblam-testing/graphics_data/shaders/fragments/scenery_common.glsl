/* Shared scenery (mod2) vertex shader. Define VERTEX_XBOX before including to
 * decode the compressed Xbox vertex format; otherwise the uncompressed PC
 * format is used. Both feed the same skinning + transform path. */

#extension GL_GOOGLE_include_directive : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;

#ifdef VERTEX_XBOX
/* Xbox: normals are packed 11_11_10, node indices are u8 (and *3 encoded),
 * and there is a single u16 weight (the second is 1 - weight0). */
layout(location = 2) in uint  i_normal;
layout(location = 3) in uint  i_binormal;
layout(location = 4) in uint  i_tangent;
layout(location = 5) in uvec2 i_node_indices;
layout(location = 6) in uint  i_node_weight0;
#else
layout(location = 2) in vec3  i_normal;
layout(location = 3) in vec3  i_binormal;
layout(location = 4) in vec3  i_tangent;
layout(location = 5) in uvec2 i_node_indices; /* bone indices (node0, node1) */
layout(location = 6) in vec2  i_node_weights; /* blend weights */
#endif

struct InstanceData {
    mat4 transform;
    int  bone_base;
    int  _pad0;
    int  _pad1;
    int  _pad2;
};

layout(binding = 0, std140) uniform MatrixStore
{
    InstanceData data[128];
} matrices;

layout(location = 1) uniform mat4 camera;

layout(binding = 3, std140) uniform BoneMatrices
{
    mat4 bones[128];
} bone_store;

layout(location = 31) uniform int render_flags;
const int RENDER_FLAG_BONES = 0x8;

layout(location = 0) out FragData {
    vec3 position;
    vec2 tex;
    vec3 tangent;
    vec3 binormal;
    vec3 normal;
    vec2 light_tex;
    flat int instanceId;
} frag;

out gl_PerVertex {
    vec4 gl_Position;
};

#include "unpack_uvec3.glsl"

void main()
{
    mat4 transform = matrices.data[gl_InstanceID].transform;
    int  bone_base = matrices.data[gl_InstanceID].bone_base;

#ifdef VERTEX_XBOX
    vec3  normal       = normalize(unpack_uvec3(i_normal)   * 2.0 - 1.0);
    vec3  binormal     = normalize(unpack_uvec3(i_binormal) * 2.0 - 1.0);
    vec3  tangent      = normalize(unpack_uvec3(i_tangent)  * 2.0 - 1.0);
    uvec2 node_indices = i_node_indices / 3u;
    /* Xbox stores node indices *3 and the weight normalized to 32767 (not
     * 65535). Single-bound verts have weight0=32767 -> w0=1, w1=0, so the
     * unused (sentinel) second node contributes nothing. */
    float w0           = clamp(float(i_node_weight0) / 32767.0, 0.0, 1.0);
    vec2  node_weights = vec2(w0, 1.0 - w0);
#else
    vec3  normal       = i_normal;
    vec3  binormal     = i_binormal;
    vec3  tangent      = i_tangent;
    uvec2 node_indices = i_node_indices;
    vec2  node_weights = i_node_weights;
#endif

    vec4 local_pos    = vec4(position, 1.0);
    vec4 local_normal = vec4(normal, 0.0);
    vec4 local_tan    = vec4(tangent, 0.0);
    vec4 local_bin    = vec4(binormal, 0.0);

#if USE_SKINNING == 1
    if(bone_base >= 0 && (render_flags & RENDER_FLAG_BONES) != 0)
    {
        mat4 b0 = bone_store.bones[bone_base + int(node_indices.x)];
        mat4 b1 = bone_store.bones[bone_base + int(node_indices.y)];
        mat4 skin = b0 * node_weights.x + b1 * node_weights.y;
        local_pos    = skin * local_pos;
        local_normal = skin * local_normal;
        local_tan    = skin * local_tan;
        local_bin    = skin * local_bin;
    }
#endif

    vec4 world_pos = transform * local_pos;
    frag.tex        = tex;
    frag.instanceId = gl_InstanceID;
    frag.tangent    = (transform * local_tan).xyz;
    frag.binormal   = (transform * local_bin).xyz;
    frag.normal     = (transform * local_normal).xyz;
    frag.position   = world_pos.xyz;
    gl_Position     = camera * world_pos;
}
