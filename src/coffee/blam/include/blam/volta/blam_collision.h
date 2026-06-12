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
    bl_string name;
    u32       flags;
    u16       material_type; /* breakable surface material enum */
    u16       padding;
    u32       unknown[8];
};

static_assert(sizeof(material) == 72);

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
    reference<collision::bsp> bsps;
};

static_assert(sizeof(node) == 64);
static_assert(offsetof(node, bsps) == 0x34);

struct header
{
    u32 flags;
    u32 indirect_damage_material;

    /* Body/shield damage parameters and effect references. Field-level
     * decoding is incomplete; the 'effe' tagrefs for body/shield damage
     * states live in this range. */
    u32 body_and_shield[139];

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
static_assert(offsetof(header, materials) == 0x234);
static_assert(offsetof(header, regions) == 0x240);
static_assert(offsetof(header, x_min) == 0x268);
static_assert(offsetof(header, pathfinding_spheres) == 0x280);
static_assert(offsetof(header, nodes) == 0x28c);

} // namespace blam::coll
