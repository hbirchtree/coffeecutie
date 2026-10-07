#pragma once

#include "blam_reference.h"
#include "blam_strings.h"

#include <peripherals/enum/helpers.h>

/* physics ('phys') tag structures: how a vehicle (or any object with one)
 * rests, slides and floats. Mass points are spheres on model nodes; the
 * powered ones push it off the ground (antigrav) or through air/water.
 *
 * Layout from Invader's physics.json definition, checked against
 * bloodgulch.map vehicles: mass point masses sum to the body's mass, and
 * positions are model space in bind pose (a warthog tire sits exactly on its
 * tire node), not relative to `model_node`, which only makes them follow
 * animation. */
namespace blam::phys {

using typing::vector_types::Vecf3;

struct powered_mass_point
{
    enum class flags_t : u32
    {
        none            = 0x0,
        ground_friction = 0x1,
        water_friction  = 0x2,
        air_friction    = 0x4,
        water_lift      = 0x8,
        air_lift        = 0x10,
        thrust          = 0x20,
        antigrav        = 0x40,
    };

    bl_string name;
    flags_t   flags;
    f32       antigrav_strength;
    f32       antigrav_offset;
    f32       antigrav_height;
    f32       antigrav_damp_fraction;
    f32       antigrav_normal_k1;
    f32       antigrav_normal_k0;
    u32       padding[17];
};

static_assert(sizeof(powered_mass_point) == 128);
static_assert(offsetof(powered_mass_point, antigrav_strength) == 0x24);
static_assert(offsetof(powered_mass_point, antigrav_normal_k0) == 0x38);

struct mass_point
{
    enum class flags_t : u32
    {
        none     = 0x0,
        metallic = 0x1,
    };

    enum class friction_t : u16
    {
        point,   /* the same in every direction */
        forward, /* rolls along `forward`, grips across it */
        left,
        up,
    };

    bl_string  name;
    i16        powered_mass_point; /* -1 = unpowered */
    i16        model_node;         /* mod2 node it follows when animated */
    flags_t    flags;
    f32        relative_mass;
    f32        mass; /* filled in by the tools */
    f32        relative_density;
    f32        density; /* filled in by the tools */
    Vecf3      position; /* model space */
    Vecf3      forward;
    Vecf3      up;
    friction_t friction_type;
    u16        padding;
    f32        friction_parallel_scale;
    f32        friction_perpendicular_scale;
    f32        radius;
    u32        padding2[5];
};

static_assert(sizeof(mass_point) == 128);
static_assert(offsetof(mass_point, position) == 0x38);
static_assert(offsetof(mass_point, friction_type) == 0x5c);
static_assert(offsetof(mass_point, radius) == 0x68);

/* Inertia tensor followed by its inverse, filled in by the tools */
struct inertial_matrix
{
    f32 m[3][3];
};

static_assert(sizeof(inertial_matrix) == 36);

struct header
{
    f32   radius;
    f32   moment_scale;
    f32   mass;
    Vecf3 center_of_mass; /* filled in by the tools */
    f32   density;
    f32   gravity_scale;
    f32   ground_friction;
    f32   ground_depth;
    f32   ground_damp_fraction;
    f32   ground_normal_k1;
    f32   ground_normal_k0;
    u32   padding;
    f32   water_friction;
    f32   water_depth;
    f32   water_density;
    u32   padding2;
    f32   air_friction;
    u32   padding3;
    Vecf3 moments; /* xx, yy, zz, filled in by the tools */

    reference<inertial_matrix>    inertial_matrix_and_inverse;
    reference<powered_mass_point> powered_mass_points;
    reference<mass_point>         mass_points;
};

static_assert(offsetof(header, gravity_scale) == 0x1c);
static_assert(offsetof(header, water_friction) == 0x38);
static_assert(offsetof(header, moments) == 0x50);
static_assert(offsetof(header, powered_mass_points) == 0x68);
static_assert(sizeof(header) == 128);

} // namespace blam::phys

C_FLAGS(blam::phys::powered_mass_point::flags_t, libc_types::u32)
C_FLAGS(blam::phys::mass_point::flags_t, libc_types::u32)
