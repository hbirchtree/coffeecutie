#pragma once

#include "blam_base_types.h"
#include "blam_tag_classes.h"
#include "blam_tag_ref.h"

namespace blam::sound {

struct pitch_permutation_t
{
    bl_string name;
    f32       skip_fraction;
    f32       gain;

    enum codec_t : u16
    {
        pcm,
        xbox_adpcm,
        ima_adpcm,
        ogg,
    } codec;

    i16 next_permutation_idx;
    u32 unknown2[2];
    u32 samples_pointer;
    u32 unknown3;

    u32 buffer_size;
    u32 sample_size;
    u32 channels_maybe;
    u32 sample_offset;
    u32 pad[4];
    u32 offset3;
    u32 empty;

    reflexive_t<char> mouth_data;
    reflexive_t<char> subtitle_data;

    reflexive_t<char> sample_data() const
    {
        return reflexive_t<char>{
            .count = sample_size,
            .offset = sample_offset,
        };
    }
};

static_assert(sizeof(pitch_permutation_t) == 124);

struct pitch_ranges_t
{
    bl_string                        name;
    f32                              natural_pitch;
    Vecf2                            bend_bounds;
    i16                              actual_permutation_count;
    f32                              playback_rate;
    i32                              padding[2];
    reflexive_t<pitch_permutation_t> permutations;
};

struct sound
{
    enum flags_t : u32
    {
        none                               = 0x0,
        fit_to_adpcm_blocksize             = 0x1,
        split_long_sound_into_permutations = 0x2,
    } flags;

    enum type_t : u16
    {
        projectile_impact,
        projectile_detonation,
        weapon_fire = 4,
        weapon_ready,
        weapon_reload,
        weapon_empty,
        weapon_charge,
        weapon_overheat,
        weapon_idle,
        object_impacts = 13,
        particle_impacts,
        slow_particle_impacts,
        unit_footsteps = 18,
        unit_dialog,
        vehicle_collision = 22,
        vehicle_engine,
        device_door = 26,
        device_force_field,
        device_machinery,
        device_nature,
        device_computers,
        music = 32,
        ambient_nature,
        ambient_machinery,
        ambient_computers,
        first_person_damage    = 39,
        scripted_dialog_player = 44,
        scripted_effect,
        scripted_dialog_other,
        scripted_dialog_force_unspatialized,
        game_event = 50,
    } type;

    enum sample_rate_t : u16
    {
        _22kHz,
        _44kHz,
    } sample_rate;

    f32 min_distance;
    f32 max_distance;
    f32 skip_fraction; /* 0 = always play, 1 = only play 1/2 times, 2 = only
                          play 1/4 times */
    Vecf2 random_pitch_bounds;
    f32   inner_cone_angle;
    f32   outer_cone_angle;
    f32   outer_cone_gain;
    f32   gain_modifier;
    f32   maximum_bend_per_second;
    u32   padding[9];
    f32   skip_fraction_modifier;
    f32   gain_modifier_;
    f32   pitch_modifier_;
    u32   padding2[3];

    enum channels_t : u16
    {
        mono,
        stereo,
    } channels;

    enum codec_t : u16
    {
        pcm,
        xbox_adpcm,
        ima_adpcm,
        ogg,
    } codec;

    tagref_typed_t<tag_class_t::snd> promotion_sound;
    i16                              promotion_count;
    i32                              maximum_play_time;
    i32                              unknown[4];
    reflexive_t<pitch_ranges_t>      pitch_ranges;
};

static_assert(sizeof(sound) == 164);

struct track_t
{
    enum flags_t : u32
    {
        none              = 0x0,
        fade_in_at_start  = 0x1,
        fade_out_at_stop  = 0x2,
        fade_in_alternate = 0x4,
    } flags;

    f32 gain;
    f32 fade_in_duration;
    f32 fade_out_duration;

    u32 padding1[8];

    tagref_typed_t<tag_class_t::snd> start;
    tagref_typed_t<tag_class_t::snd> loop;
    tagref_typed_t<tag_class_t::snd> end;

    u32 padding2[8];

    tagref_typed_t<tag_class_t::snd> alternate_loop;
    tagref_typed_t<tag_class_t::snd> alternate_end;
};

static_assert(sizeof(track_t) == 160);

struct detail_sound_t
{
    tagref_typed_t<tag_class_t::snd> sound;
    Vecf2                            random_period_bounds;
    f32                              gain;

    enum flags_t : u32
    {
        none                        = 0x0,
        dont_play_with_alternate    = 0x1,
        dont_play_without_alternate = 0x2,
    } flags;

    u32 padding[12];

    Vecf2 yaw_bounds;
    Vecf2 pitch_bounds;
    Vecf2 distance_bounds;
};

static_assert(sizeof(detail_sound_t) == 104);

struct looping_sound
{
    enum flags_t : u32
    {
        none            = 0x0,
        defeaning_to_ai = 0x1,
        not_a_loop      = 0x2,
        stops_music     = 0x4,
    } flags;

    f32 zero_detail_sound_period;

    u32 padding_because_effort[13];

    reflexive_t<track_t>        tracks;
    reflexive_t<detail_sound_t> detail_sounds;
};

static_assert(sizeof(looping_sound) == 84);

} // namespace blam::sound
