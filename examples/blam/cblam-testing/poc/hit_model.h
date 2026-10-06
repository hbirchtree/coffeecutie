#pragma once

/* Biped hit geometry straight from the biped's coll tag: one or more solid
 * BSPs per collision node, in node space, placed by the mod2 bind pose. Each
 * BSP surface names a coll material, and the material says whether it is the
 * head and how it scales shield/body damage. */

#include "events.h"

#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_collision.h>
#include <blam/volta/blam_mod2.h>

#include <optional>
#include <string>
#include <vector>

namespace poc {

struct HitMaterial
{
    std::string name;
    bool        head{false};
    float       shield_mult{1.f};
    float       body_mult{1.f};
};

struct HitNode
{
    std::string                                name;
    std::int16_t                               parent{-1};
    mat4                                       bind{1.f}; /* node -> biped */
    mat4                                       inv_bind{1.f};
    std::vector<blam::collision::model_bsp const*> bsps;
    vec3                                       lo{0.f}; /* node space bounds */
    vec3                                       hi{0.f};
    vec3                                       center{0.f}; /* node space */
    float                                      radius{0.f};
    std::int16_t                               material{-1}; /* most used */
};

struct HitModel
{
    blam::map_ptr            magic;
    std::vector<HitNode>     nodes;
    std::vector<HitMaterial> materials;

    vec3  bound_center{0.f}; /* biped space */
    float bound_radius{0.f};
    vec3  head_center{0.f}; /* biped space, for aiming */
    vec3  body_center{0.f};
    float max_body{75.f};
    float max_shield{75.f};

    struct Hit
    {
        float        t;
        std::int16_t node;
        std::int16_t material;
        vec3         normal; /* world */
        vec3         local;  /* node space */
    };

    static mat4 root(BipedPose const& pose);

    std::optional<Hit> raycast(
        BipedPose const& pose, vec3 const& start, vec3 const& end) const;

    vec3 node_to_world(BipedPose const& pose, std::int16_t node, vec3 local)
        const;

    HitMaterial const* material(std::int16_t idx) const
    {
        return idx >= 0 && static_cast<size_t>(idx) < materials.size()
                   ? &materials[idx]
                   : nullptr;
    }

    void dump() const;

    /* Fills nodes' spheres/materials and the model-wide aim points once the
     * nodes, bind poses and materials are in. */
    void finalize();
};

/* Builds from coll + mod2 (bind pose matched to coll nodes by name). */
std::optional<HitModel> build_hit_model(
    blam::coll::header const&       coll,
    blam::mod2::bone const*         bones,
    size_t                          bone_count,
    blam::map_ptr const&            magic);

/* The structure BSP's collision, for terrain hits. */
struct Terrain
{
    blam::collision::bsp const* bsp{nullptr};
    blam::bsp_ptr               magic;

    std::optional<blam::collision::ray_hit> raycast(
        vec3 const& start, vec3 const& end) const
    {
        if(!bsp)
            return std::nullopt;
        return bsp->raycast(start, end, magic);
    }

    /* Ground height under (x, y), searching down from z */
    std::optional<vec3> ground(vec3 const& from, float depth = 20.f) const;
};

/* What the PoC needs from a loaded map: the player biped's hit model, the
 * structure BSP's collision and the player starts. Points into the map's
 * memory, so it lives no longer than the map. See load_world() in world.h. */
struct World
{
    std::string            biped_name;
    HitModel               model;
    Terrain                terrain;
    std::vector<BipedPose> starts;
};

} // namespace poc
