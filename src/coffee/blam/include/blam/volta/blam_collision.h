#pragma once

#include "blam_bsp_structures.h"
#include "blam_reference.h"
#include "blam_strings.h"
#include "blam_tag_index.h"

/* model_collision_geometry ('coll') tag structures.
 *
 * Layout reverse-engineered from b30.map (Xbox): offsets verified against a
 * dump of 'scenery\trees\tree_leafy\tree_leafy' (4 nodes, 2 materials,
 * 2 regions, 1 pathfinding sphere). Each collision node owns zero or more
 * collision::bsp blocks — the exact same structure the structure BSP's
 * collision_header points to (see blam_bsp_structures.h).
 */
namespace blam::coll {

using typing::vector_types::Vecf3;

struct material
{
    enum flags_t : u32
    {
        none = 0x0,
        head = 0x1,
    };

    bl_string name;
    flags_t   flags;
    u16       material_type; /* breakable surface material enum */
    u16       padding;
    f32       shield_leak_percentage;
    f32       shield_damage_multiplier;
    u32       padding2[3];
    f32       body_damage_multiplier;
    u32       padding3[2];
};

static_assert(sizeof(material) == 72);
static_assert(offsetof(material, body_damage_multiplier) == 0x3c);

struct permutation
{
    bl_string name;
};

static_assert(sizeof(permutation) == 32);

struct region
{
    bl_string              name;
    u32                    flags;
    u32                    padding;
    f32                    damage_threshold;
    u32                    padding2[3];
    tagref_t               destroyed_effect;
    reference<permutation> permutations;
};

static_assert(sizeof(region) == 84);
static_assert(offsetof(region, destroyed_effect) == 0x38);
static_assert(offsetof(region, permutations) == 0x48);

struct pathfinding_sphere
{
    i16   node; /* collision node the sphere follows, -1 = none */
    u16   padding[7];
    Vecf3 center;
    f32   radius;
};

static_assert(sizeof(pathfinding_sphere) == 32);

struct node
{
    bl_string name;
    u16       region;       /* region this node belongs to, -1 = none */
    i16       parent;       /* parent node index, -1 = root */
    i16       next_sibling; /* -1 = last */
    i16       first_child;  /* -1 = leaf of the node hierarchy */
    u32       padding[3];
    /* Solid-leaf BSP(s) for this node, same structure as the structure
     * BSP's collision (bsp3d nodes/planes/leaves/bsp2d/surfaces/edges/
     * vertices). Usually 1 for solid nodes, 0 for purely structural ones. */
    reference<collision::model_bsp> bsps;
};

static_assert(sizeof(node) == 64);
static_assert(offsetof(node, bsps) == 0x34);

struct header
{
    u32 flags;
    u32 indirect_damage_material;

    /* Body: what it takes, and the effects as it is damaged and destroyed */
    f32      maximum_body_vitality;
    f32      body_system_shock;
    u32      padding0[13];
    f32      friendly_damage_resistance;
    u32      padding1[10];
    tagref_t localized_damage_effect;
    f32      area_damage_effect_threshold;
    tagref_t area_damage_effect;
    f32      body_damaged_threshold;
    tagref_t body_damaged_effect;
    tagref_t body_depleted_effect;
    f32      body_destroyed_threshold;
    tagref_t body_destroyed_effect;

    /* Shield: what it soaks up, and how it comes back after a hit */
    f32      maximum_shield_vitality;
    u16      padding2;
    u16      shield_material_type;
    u32      padding3[6];
    u16      shield_failure_function;
    u16      padding4;
    f32      shield_failure_threshold;
    f32      failing_shield_leak_fraction;
    u32      padding5[4];
    f32      minimum_stun_damage;
    f32      stun_time;     /* seconds before it starts to recharge */
    f32      recharge_time; /* seconds from empty to full */
    u32      padding6[28];
    f32      shield_damaged_threshold;
    tagref_t shield_damaged_effect;
    tagref_t shield_depleted_effect;
    tagref_t shield_recharging_effect;
    u32      padding7[31];

    reference<material> materials;
    reference<region>   regions;
    reference<byte_t>   modifiers; /* unused in shipping maps */
    u32                 padding[4];

    /* Pathfinding bounds (AI obstacle box around the object) */
    f32 x_min, x_max;
    f32 y_min, y_max;
    f32 z_min, z_max;

    reference<pathfinding_sphere> pathfinding_spheres;
    reference<node>               nodes;
};

static_assert(sizeof(header) == 664);
static_assert(offsetof(header, maximum_body_vitality) == 0x08);
static_assert(offsetof(header, localized_damage_effect) == 0x70);
static_assert(offsetof(header, body_destroyed_effect) == 0xbc);
static_assert(offsetof(header, maximum_shield_vitality) == 0xcc);
static_assert(offsetof(header, stun_time) == 0x10c);
static_assert(offsetof(header, shield_damaged_effect) == 0x188);
static_assert(offsetof(header, materials) == 0x234);
static_assert(offsetof(header, regions) == 0x240);
static_assert(offsetof(header, x_min) == 0x268);
static_assert(offsetof(header, pathfinding_spheres) == 0x280);
static_assert(offsetof(header, nodes) == 0x28c);

} // namespace blam::coll
