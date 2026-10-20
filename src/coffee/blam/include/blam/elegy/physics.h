#pragma once

#include "structures.h"

#include <optional>
#include <vector>

namespace blam::elegy::phys {

/*! Havok shape kinds, as rigid bodies and containers name their shapes */
enum class shape_type_t : i16
{
    sphere       = 0,
    pill         = 1,
    box          = 2,
    triangle     = 3,
    polyhedron   = 4,
    multi_sphere = 5,
    phantom      = 6,
    list         = 14,
    mopp         = 15,
};

enum class motion_type_t : u16
{
    sphere            = 0,
    stabilized_sphere = 1,
    box               = 2,
    stabilized_box    = 3,
    keyframed         = 4,
    fixed             = 5,
};

/*! A Havok shape's runtime header, kept in the tag */
struct havok_shape_t
{
    u32     runtime;
    be<i16> size;
    be<i16> count;
    be<u32> user_data;
    be<i32> type;
};

static_assert(sizeof(havok_shape_t) == 0x10);

/*! What every primitive starts with: its material and mass properties */
struct shape_base_t
{
    string_id     name;
    be<i16>       material;
    be<u16>       flags;
    be<f32>       relative_mass_scale;
    be<f32>       friction;
    be<f32>       restitution;
    be<f32>       volume;
    be<f32>       mass;
    be<i16>       mass_distribution;
    i8            phantom_type;
    u8            collision_group;
    havok_shape_t havok;
    be<f32>       radius; /*!< Convex radius, the shell around the shape */
    std::array<u8, 0xC> unknown_0;
};

static_assert(sizeof(shape_base_t) == 0x40);

/*! A sphere, inside a translate shape */
struct sphere_t
{
    shape_base_t  base;
    havok_shape_t translate;
    be<f32>       translate_radius;
    u32           runtime;
    be<i16>       child_type;
    be<i16>       child_index;
    be<i32>       child_size;
    be<Vecf4>     translation;
};

static_assert(sizeof(sphere_t) == 0x70);

/*! A capsule between two points, base.radius thick */
struct pill_t
{
    shape_base_t base;
    be<Vecf4>    bottom;
    be<Vecf4>    top;
};

static_assert(sizeof(pill_t) == 0x60);

/*! A box, inside a transform shape (rotation rows and translation) */
struct box_t
{
    shape_base_t            base;
    be<Vecf4>               half_extents;
    havok_shape_t           transform;
    be<f32>                 transform_radius;
    u32                     runtime;
    be<i16>                 child_type;
    be<i16>                 child_index;
    be<i32>                 child_size;
    std::array<be<Vecf4>, 3> rotation;
    be<Vecf4>               translation;
};

static_assert(sizeof(box_t) == 0xB0);

struct triangle_t
{
    shape_base_t             base;
    std::array<be<Vecf4>, 3> points;
    be<Vecf4>                extrusion;
};

static_assert(sizeof(triangle_t) == 0x80);

/*! A convex hull; its corners are the next `four_vectors_size` entries of
 * the model's four vectors, four corners transposed into each */
struct polyhedron_t
{
    shape_base_t base;
    be<Vecf4>    aabb_half_extents;
    be<Vecf4>    aabb_center;
    u32          runtime_0;
    be<i32>      four_vectors_size;
    be<u32>      four_vectors_capacity;
    be<i32>      vertex_count;
    u32          runtime_1;
    be<i32>      planes_size;
    be<u32>      planes_capacity;
    be<i32>      connectivity;
};

static_assert(sizeof(polyhedron_t) == 0x80);

/*! Four polyhedron corners: their x, y and z components */
struct four_vectors_t
{
    be<Vecf4> x;
    be<Vecf4> y;
    be<Vecf4> z;
};

static_assert(sizeof(four_vectors_t) == 0x30);

/*! Up to eight spheres, centre and radius in w */
struct multi_sphere_t
{
    shape_base_t             base;
    std::array<be<Vecf4>, 8> spheres;
};

static_assert(offsetof(multi_sphere_t, spheres) == 0x40);
static_assert(sizeof(multi_sphere_t) == 0xC0);

/*! A container; its children are the next `child_count` list shapes */
struct list_t
{
    havok_shape_t       havok;
    u32                 runtime_0;
    i8                  disable_welding;
    i8                  collection_type;
    be<i16>             unknown_0;
    u32                 runtime_1;
    be<i32>             child_count;
    be<u32>             child_capacity;
    std::array<u8, 0xC> unknown_1;
    be<Vecf4>           aabb_half_extents;
    be<Vecf4>           aabb_center;
};

static_assert(offsetof(list_t, child_count) == 0x1C);
static_assert(sizeof(list_t) == 0x50);

struct list_shape_t
{
    be<i16> type;
    be<i16> index;
    u32     unknown_0;
    be<u32> unknown_1;
    be<i32> child_count;
};

static_assert(sizeof(list_shape_t) == 0x10);

/*! Havok's MOPP tree over a child shape */
struct mopp_t
{
    havok_shape_t havok;
    u32           runtime_0;
    be<i16>       child_type;
    be<i16>       child_index;
    u32           runtime_1;
    u32           unknown_0;
};

static_assert(sizeof(mopp_t) == 0x20);

struct rigid_body_t
{
    be<i16>                  node;
    be<i16>                  region;
    be<i16>                  permutation;
    be<i16>                  unknown_0;
    be<Vecf3>                bounding_sphere_offset;
    be<f32>                  bounding_sphere_radius;
    be<u16>                  flags;
    be<motion_type_t>        motion_type;
    be<i16>                  no_phantom_power_alternative;
    be<i16>                  size;
    be<f32>                  inertia_tensor_scale;
    be<f32>                  linear_damping;
    be<f32>                  angular_damping;
    be<Vecf3>                center_of_mass_offset;
    std::array<be<f32>, 6>   water_physics;
    std::array<u8, 8>        unknown_1;
    be<i16>                  shape_type;
    be<i16>                  shape_index;
    be<f32>                  mass;
    be<Vecf4>                center_of_mass;
    std::array<be<Vecf4>, 3> inertia_tensor;
    be<f32>                  bounding_sphere_pad;
    std::array<u8, 8>        unknown_2;
    i8                       collision_quality_override;
    i8                       unknown_3;
    be<u16>                  runtime_flags;
};

static_assert(offsetof(rigid_body_t, shape_type) == 0x58);
static_assert(offsetof(rigid_body_t, center_of_mass) == 0x60);
static_assert(sizeof(rigid_body_t) == 0xB0);

/*! A shape's material: a global material (matg) by name */
struct material_t
{
    string_id name;
    string_id global_material;
    be<i16>   phantom_type;
    u8        proxy_collision_group;
    u8        collision_group;

    /*! The global material it is made of; its own name when none is given */
    inline string_id global_name() const
    {
        return global_material.valid() ? global_material : name;
    }
};

static_assert(sizeof(material_t) == 0xC);

/*! physics_model ("phmo"): rigid bodies built from Havok shapes */
struct physics_model_t
{
    static constexpr tag_class_t group = tag_class_t::phmo;

    be<u32>                        flags;
    be<f32>                        mass;
    be<f32>                        low_frequency_deactivation_scale;
    be<f32>                        high_frequency_deactivation_scale;
    be<f32>                        custom_shape_radius;
    be<f32>                        maximum_penetration_depth_scale;
    i8                             import_version;
    i8                             unknown_0;
    be<i16>                        unknown_1;
    std::array<u8, 0x3C>           unknown_2;
    tag_block<rigid_body_t>        rigid_bodies;
    tag_block<material_t>          materials;
    tag_block<sphere_t>            spheres;
    tag_block<multi_sphere_t>      multi_spheres;
    tag_block<pill_t>              pills;
    tag_block<box_t>               boxes;
    tag_block<triangle_t>          triangles;
    tag_block<polyhedron_t>        polyhedra;
    tag_block<four_vectors_t>      four_vectors;
    tag_block<std::array<u8, 0x10>> plane_equations;
    tag_block<std::array<u8, 0x40>> mass_distributions;
    tag_block<list_t>              lists;
    tag_block<list_shape_t>        list_shapes;
    tag_block<mopp_t>              mopps;

    /*! A shape's material, by the index the shape stores */
    inline material_t const* material(tag_magic const& magic, i16 index) const
    {
        auto all = materials.span(magic);
        return index >= 0 && static_cast<size_t>(index) < all.size()
                   ? &all[static_cast<size_t>(index)]
                   : nullptr;
    }
};

static_assert(offsetof(physics_model_t, rigid_bodies) == 0x58);
static_assert(offsetof(physics_model_t, polyhedra) == 0xAC);
static_assert(offsetof(physics_model_t, mopps) == 0xF4);

/*! A global material (matg): what it is made of, physically */
struct global_material_t
{
    string_id             name;
    string_id             parent;
    be<i16>               index;
    be<u16>               flags;
    string_id             general_armor;
    string_id             specific_armor;
    be<u32>               physics_flags;
    be<f32>               friction;
    be<f32>               restitution;
    be<f32>               density;
    std::array<u8, 0x14C> unknown_0;
};

static_assert(offsetof(global_material_t, friction) == 0x18);
static_assert(sizeof(global_material_t) == 0x170);

/*! globals ("matg"), as far as physics needs it */
/*! How a kind of player is drawn: first-person hands and body, and the
 * unit seen from outside */
struct player_representation_t
{
    tag_reference first_person_hands; /*!< mode */
    tag_reference first_person_body;  /*!< mode */
    tag_reference third_person_unit;
    string_id     third_person_variant;
    tag_reference binoculars_zoom_in;
    tag_reference binoculars_zoom_out;
};

static_assert(sizeof(player_representation_t) == 0x54);

struct globals_t
{
    static constexpr tag_class_t group = tag_class_t::matg;

    std::array<u8, 0x164>                      unknown_0;
    tag_block<player_representation_t>         player_representations;
    std::array<u8, 0xC>                        unknown_1;
    tag_block<global_material_t>               materials;

    /*! A material by name, as physics models name them */
    inline global_material_t const* material(tag_magic const& magic, string_id name) const
    {
        for(auto const& material : materials.span(magic))
            if(material.name.raw.value() == name.raw.value())
                return &material;
        return nullptr;
    }
};

static_assert(offsetof(globals_t, player_representations) == 0x164);
static_assert(offsetof(globals_t, materials) == 0x17C);

/*! A rigid body's shapes as convex point sets, in the body's node space */
struct hull_t
{
    std::vector<Vecf3> points;
    i16                material{-1};
    f32                friction{0.f};
    f32                restitution{0.f};
    f32                mass{0.f};
    f32                radius{0.f}; /*!< Havok's convex shell around the points */
};

/*! Flattens a rigid body's shape tree (lists, MOPPs) into hulls; spheres and
 * pills become sampled points */
std::vector<hull_t> hulls_of(
    physics_model_t const& model, tag_magic const& magic, rigid_body_t const& body);

/*! A permutation's rigid bodies as one body that moves */
struct body_t
{
    std::vector<hull_t> hulls; /*!< Object space */
    f32                 mass{0.f};
    Vecf3               center_of_mass{};
    f32                 friction{0.f};    /*!< Weighted by each shape's mass */
    f32                 restitution{0.f};
};

/*! The rigid bodies of a permutation merged into one, if they can move as
 * one: on the root node, none fixed or keyframed */
std::optional<body_t> body_of(
    physics_model_t const& model, tag_magic const& magic, i16 permutation = 0);

} // namespace blam::elegy::phys
