#pragma once

/* Posed models on GX, which transforms a vertex by one of ten matrices it
 * names. Each vertex hangs off a node, or two with a weight; a part's
 * triangles are split into batches whose vertices need at most ten such
 * matrices, the two-node ones blended on the CPU (weights in quarters so few
 * blends are needed). Per frame, a pose gives each node's skin matrix. */

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <map>
#include <vector>

namespace gx_skin {

using libc_types::f32;
using libc_types::i16;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

constexpr u32 palette_size = 10; // GX_PNMTX0..9

/* A palette matrix: node a's skin matrix, or a mix toward b */
struct key_t
{
    u8 a{0}, b{0};
    u8 quarters{4}; // a's share

    auto operator<=>(key_t const&) const = default;
};

template<typename Vertex>
inline key_t key_of(Vertex const& v)
{
    u32 const share = (v.weight * 4u + 127u) / 255u;
    if(v.node1 == 0xFF || share >= 4 || v.node1 == v.node0)
        return {v.node0, v.node0, 4};
    if(share == 0)
        return {v.node1, v.node1, 4};
    return {v.node0, v.node1, static_cast<u8>(share)};
}

/* A vertex as a batch draws it: GX_PNMTXn first */
template<typename Vertex>
struct batch_vertex
{
    u8     matrix; // n * 3
    u8     pad[3];
    Vertex v;
};

template<typename Vertex>
struct batch_t
{
    std::vector<batch_vertex<Vertex>> verts;
    std::vector<u16>                  triangles;
    std::vector<key_t>                keys;
};

/* A triangle strip's triangles, in batches of palette_size matrices.
 * Sorted by the matrices they need, so triangles sharing them land together;
 * each batch then takes every one that still fits, in one pass */
template<typename Vertex>
inline std::vector<batch_t<Vertex>> split(
    std::vector<Vertex> const& verts, std::vector<u16> const& strip)
{
    struct tri_t
    {
        u16   index[3];
        key_t key[3];
        key_t sorted[3];
    };

    std::vector<tri_t> tris;
    for(size_t i = 0; i + 2 < strip.size(); i++)
    {
        tri_t t{{strip[i], strip[i + 1], strip[i + 2]}, {}, {}};
        if(t.index[0] == t.index[1] || t.index[1] == t.index[2] ||
           t.index[0] == t.index[2])
            continue;
        if(t.index[0] >= verts.size() || t.index[1] >= verts.size() ||
           t.index[2] >= verts.size())
            continue;
        if(i & 1)
            std::swap(t.index[0], t.index[1]); // strips alternate winding
        for(u32 k = 0; k < 3; k++)
            t.sorted[k] = t.key[k] = key_of(verts[t.index[k]]);
        std::sort(t.sorted, t.sorted + 3);
        tris.push_back(t);
    }
    std::stable_sort(
        tris.begin(), tris.end(), [](tri_t const& x, tri_t const& y) {
            return std::lexicographical_compare(
                x.sorted, x.sorted + 3, y.sorted, y.sorted + 3);
        });

    std::vector<batch_t<Vertex>> out;
    std::vector<bool>            done(tris.size(), false);
    size_t                       left = tris.size();
    while(left)
    {
        auto&              b = out.emplace_back();
        std::map<u16, u16> remap;
        for(size_t i = 0; i < tris.size(); i++)
        {
            if(done[i])
                continue;
            auto const& t     = tris[i];
            u32         fresh = 0;
            for(u32 k = 0; k < 3; k++)
                fresh += std::find(b.keys.begin(), b.keys.end(), t.key[k]) ==
                             b.keys.end() &&
                         std::find(t.key, t.key + k, t.key[k]) == t.key + k;
            if(b.keys.size() + fresh > palette_size)
                continue;
            for(u32 k = 0; k < 3; k++)
            {
                auto it = std::find(b.keys.begin(), b.keys.end(), t.key[k]);
                if(it == b.keys.end())
                    it = b.keys.insert(b.keys.end(), t.key[k]);
                u8 const slot    = static_cast<u8>(it - b.keys.begin());
                auto [at, added] = remap.try_emplace(
                    t.index[k], static_cast<u16>(b.verts.size()));
                if(added)
                    b.verts.push_back(
                        {static_cast<u8>(slot * 3), {}, verts[t.index[k]]});
                b.triangles.push_back(at->second);
            }
            done[i] = true;
            left--;
        }
    }
    return out;
}

/* A model's nodes in their bind pose, rotations conjugated like antr's */
struct bone_t
{
    i16       parent{-1};
    glm::vec3 translation{0.f};
    glm::quat rotation{1.f, 0.f, 0.f, 0.f};
};

struct skeleton_t
{
    std::vector<bone_t>    bones;
    std::vector<glm::mat4> bind_inverse;
};

/* Model space of each node, from node-local transforms; parents come first */
inline std::vector<glm::mat4> model_space(
    std::vector<bone_t> const&    bones,
    std::vector<glm::quat> const* rot   = nullptr,
    std::vector<glm::vec3> const* trans = nullptr)
{
    std::vector<glm::mat4> out(bones.size(), glm::mat4{1.f});
    for(size_t i = 0; i < bones.size(); i++)
    {
        bool const posed = rot && i < rot->size() && trans && i < trans->size();
        glm::vec3 const t     = posed ? (*trans)[i] : bones[i].translation;
        glm::quat const r     = posed ? (*rot)[i] : bones[i].rotation;
        glm::mat4 const local = glm::translate(t) * glm::mat4_cast(r);
        i16 const       p     = bones[i].parent;
        out[i] = p >= 0 && static_cast<size_t>(p) < i ? out[p] * local : local;
    }
    return out;
}

inline void bind(skeleton_t& s)
{
    s.bind_inverse = model_space(s.bones);
    for(auto& m : s.bind_inverse)
        m = glm::inverse(m);
}

/* Each node's skin matrix for a pose: from bind pose to posed, model space */
inline std::vector<glm::mat4> skin(
    skeleton_t const&             s,
    std::vector<glm::quat> const& rot,
    std::vector<glm::vec3> const& trans)
{
    auto out = model_space(s.bones, &rot, &trans);
    for(size_t i = 0; i < out.size(); i++)
        out[i] = out[i] * s.bind_inverse[i];
    return out;
}

inline glm::mat4 matrix(key_t const& k, std::vector<glm::mat4> const& skin)
{
    if(k.a >= skin.size() || k.b >= skin.size())
        return glm::mat4{1.f};
    if(k.quarters == 4)
        return skin[k.a];
    f32 const w = k.quarters / 4.f;
    return skin[k.a] * w + skin[k.b] * (1.f - w);
}

} // namespace gx_skin
