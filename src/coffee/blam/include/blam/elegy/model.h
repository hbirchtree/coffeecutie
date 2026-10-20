#pragma once

#include "geometry.h"

namespace blam::elegy {

/*! A run of meshes in render_geometry_t::meshes */
struct permutation_t
{
    string_id                name;
    block_index<mesh_t>      mesh;
    be<i16>                  mesh_count;
    std::array<be<u32>, 2>   instance_flags;
};

static_assert(sizeof(permutation_t) == 0x10);

struct region_t
{
    string_id                name;
    tag_block<permutation_t> permutations;
};

static_assert(sizeof(region_t) == 0x10);

struct instance_placement_t
{
    string_id name;
    be<i32>   node;
    be<f32>   scale;
    be<Vecf3> forward;
    be<Vecf3> left;
    be<Vecf3> up;
    be<Vecf3> position;
};

static_assert(sizeof(instance_placement_t) == 0x3C);

struct node_t
{
    string_id             name;
    block_index<node_t>   parent;
    block_index<node_t>   first_child;
    block_index<node_t>   next_sibling;
    u16                   unknown_0;
    be<Vecf3>             default_translation;
    be<Vecf4>             default_rotation;
    be<f32>               inverse_scale;
    be<Vecf3>             inverse_forward;
    be<Vecf3>             inverse_left;
    be<Vecf3>             inverse_up;
    be<Vecf3>             inverse_position;
    be<f32>               distance_from_parent;
};

static_assert(sizeof(node_t) == 0x60);

struct marker_t
{
    i8        region;
    i8        permutation;
    i8        node;
    u8        unknown_0;
    be<Vecf3> translation;
    be<Vecf4> rotation;
    be<f32>   scale;
};

static_assert(sizeof(marker_t) == 0x24);

struct marker_group_t
{
    string_id           name;
    tag_block<marker_t> markers;
};

static_assert(sizeof(marker_group_t) == 0x10);

/*! A render method plus per-material overrides */
struct geometry_material_t
{
    tag_reference<>                   render_method;
    tag_block<std::array<u8, 0xC>>  properties;
    be<i32>                         imported_material;
    i8                              breakable_surface;
    u8                              unknown_0;
    u16                             unknown_1;
};

static_assert(sizeof(geometry_material_t) == 0x24);

/*! render_model ("mode") */
struct render_model_t
{
    static constexpr tag_class_t group = tag_class_t::mode;

    string_id                            name;
    be<u16>                              flags;
    u16                                  unknown_0;
    be<i32>                              checksum;
    tag_block<region_t>                  regions;
    std::array<u32, 2>                   unknown_1;
    tag_block<instance_placement_t>      instance_placements;
    be<i32>                              node_checksum;
    tag_block<node_t>                    nodes;
    tag_block<marker_group_t>            marker_groups;
    tag_block<geometry_material_t>       materials;
    tag_block<std::array<u8, 1>>         errors;
    be<f32>                              dont_draw_over_camera_cosine;
    render_geometry_t                    geometry;
};

static_assert(offsetof(render_model_t, regions) == 0xC);
static_assert(offsetof(render_model_t, instance_placements) == 0x20);
static_assert(offsetof(render_model_t, nodes) == 0x30);
static_assert(offsetof(render_model_t, materials) == 0x48);
static_assert(offsetof(render_model_t, geometry) == 0x64);

} // namespace blam::elegy
