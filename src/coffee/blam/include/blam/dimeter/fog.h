#pragma once

#include "structures.h"

namespace blam::dimeter::fog {

/*!
 * Planar fog (fog tag), layout from Assembly's Halo2 fog.xml: a volume
 * under one of the structure BSP's fog planes, opaque `opaque_depth`
 * below the plane and `opaque_distance` into it.
 */
struct alignas(4) patchy_fog_t
{
    Vecf3           color;
    u32             unknown_0[3];
    Vecf2           density;
    Vecf2           distance;
    f32             minimum_depth_fraction;
    tag_reference<> patchy_fog;
};

static_assert(sizeof(patchy_fog_t) == 0x34);

struct alignas(4) header_t
{
    u16                       flags;
    i16                       priority;
    string_id                 global_material_name;
    i16                       global_material_index;
    i16                       unknown_0;
    f32                       maximum_density;
    f32                       opaque_distance;
    f32                       opaque_depth;
    Vecf2                     atmospheric_planar_depth;
    f32                       eye_offset_scale;
    Vecf3                     color;
    tag_block<patchy_fog_t>   patchy_fog;
    tag_reference<tag_class_t::lsnd> background_sound;
    tag_reference<tag_class_t::snde> sound_environment;
    f32                       environment_damping_factor;
    f32                       background_sound_gain;
    tag_reference<tag_class_t::snd>  enter_sound;
    tag_reference<tag_class_t::snd>  exit_sound;
};

static_assert(offsetof(header_t, color) == 0x24);
static_assert(offsetof(header_t, background_sound) == 0x38);
static_assert(sizeof(header_t) == 0x60);

} // namespace blam::dimeter::fog
