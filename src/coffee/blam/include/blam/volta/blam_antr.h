#pragma once

#include "blam/volta/blam_tag_classes.h"
#include "blam/volta/blam_tag_ref.h"
#include "blam_base_types.h"
#include "blam_reference.h"
#include "blam_strings.h"
#include "blam_structures.h"

namespace blam::antr {

/* 20-byte Halo tag-data block:
 * [0x00] size  [0x04] flags  [0x08] file_offset  [0x0C] pointer  [0x10] unused
 * The in-memory pointer (0x0C) is the magic-relative address of the data. */
struct data_ref_t
{
    u32 count;       /* 0x00: byte count */
    u32 flags;       /* 0x04 */
    u32 file_offset; /* 0x08: offset within map file */
    u32 pointer;     /* 0x0C: in-memory address (use with magic) */
    u32 unused;      /* 0x10 */

    auto data(map_ptr const& magic) const
    {
        return reference<u8>{count, pointer}.data(magic);
    }

    u32 size() const
    {
        return count;
    }
};

static_assert(sizeof(data_ref_t) == 20);

enum class frame_info_type : u16
{
    none          = 0,
    dx_dy         = 1,
    dx_dy_dyaw    = 2,
    dx_dy_dz_dyaw = 3,
};

enum class anim_type : u16
{
    base        = 0,
    overlay     = 1,
    replacement = 2,
};

enum class anim_flags : u16
{
    compressed_data = 0x1,
    world_relative  = 0x2,
    pal_25hz        = 0x4,
};

struct animation_ref
{
    // Points into the animation array on the parent header
    i16 animation;
};

struct screen_bounds
{
    f32 right_yaw_per_frame;
    f32 left_yaw_per_frame;
    u32 right_frame_count;
    u32 left_frame_count;
    f32 down_pitch_per_frame;
    f32 up_pitch_per_frame;
    u32 down_pitch_frame_count;
    u32 up_pitch_frame_count;
};
static_assert(sizeof(screen_bounds) == 32);

struct node_t
{
    bl_string name;
    i16       next_sibling;
    i16       first_child;
    i16       parent;
    i16       padding1;
    enum node_joint_flags_t : u16
    {
        ball_socket = 0x1,
        hinge       = 0x2,
        no_movement = 0x4,
    } joint_flags;
    Vecf3     base_vector;
    f32       vector_range;
    u32       padding2;
};
static_assert(sizeof(node_t) == 64);

/* Per-node compressed quaternion (4×i16, range [-1,1] mapped to i16) */
struct compressed_quat_t
{
    i16 x, y, z, w;

    Quatf decompress() const
    {
        constexpr f32 scale = 1.0f / 32767.0f;
        return glm::normalize(
            Quatf(w * scale, x * scale, y * scale, z * scale));
    }
};

static_assert(sizeof(compressed_quat_t) == 8);

struct object
{
    animation_ref animation;
    enum function_t : u16
    {
        A_out,
        B_out,
        C_out,
        D_out,
    } function;
    enum function_controls_t : u16
    {
        frame,
        scale,
    };
    u32 padding[4];
};
static_assert(sizeof(object) == 20);

struct ik_point
{
    bl_string marker; // the marker on the object being attached
    bl_string attach_to_marker; // the marker name on the object being attached to
};
static_assert(sizeof(ik_point) == 64);

enum class weapon_animation_idx_t
{
        reload_1,
        reload_2,
        chamber_1,
        chamber_2,
        fire_1,
        fire_2,
        charged_1,
        charged_2,
        melee,
        overheat,
};

struct weapon_type
{
    bl_string                label;
    reference<animation_ref> animations; // indexed by weapon_animation_idx_t
};

struct unit_weapon
{
    bl_string name;
    bl_string grip_marker;
    bl_string hand_marker;

    /* aiming screen bounds */
    screen_bounds aiming_bounds;
    u32 padding[6];

    enum animation_idx_t
    {
        idle,
        posture,
        turn_left,
        turn_right,
        dive_front,
        dive_back,
        dive_left,
        dive_right,
        move_front,
        move_back,
        move_left,
        move_right,
        slide_front,
        slide_back,
        slide_left,
        slide_right,
        airborne,
        land_soft,
        land_hard,
        unused,
        throw_grenade,
        disarm,
        drop,
        ready,
        put_away,
        aim_still,
        aim_move,
        surprise_front,
        surprise_back,
        berserk,
        evade_left,
        evade_right,
        signal_move,
        signal_attack,
        warn,
        stunned_front,
        stunned_back,
        stunned_left,
        stunned_right,
        melee,
        celebrate,
        panic,
        melee_airborne,
        flaming,
        resurrect_front,
        resurrect_back,
        melee_continuous,
        feeding,
        lean_start,
        lean_airborne,
        lean_melee,
        zapping,
    };
    reference<animation_ref>   animations;
    reference<ik_point>    ik_points;
    reference<weapon_type> weapon_types;
};
static_assert(sizeof(unit_weapon) == 188);

struct unit
{
    bl_string label;
    /* looking screen bounds */
    screen_bounds looking_bounds;

    // indexes animations
    // source: guerilla
    enum animation_idx_t
    {
        airborne_dead,
        landing_dead,
        acc_front_back,
        acc_left_right,
        acc_up_down,
        push,
        twist,
        enter,
        exit,
        look,
        talk,
        emotions,
        unused,
        user0,
        user1,
        user2,
        user3,
        user4,
        user5,
        user6,
        user7,
        user8,
        user9,
        flying_front,
        flying_back,
        flying_left,
        flying_right,
        opening,
        closing,
        hovering,
    };
    reference<animation_ref> animations;
    reference<ik_point>      ik_points;
    reference<unit_weapon>   weapons;
};
static_assert(sizeof(unit) == 100);

struct weapon
{
    reference<animation_ref> animations; // indexed by weapon_animation_idx_t
};
static_assert(sizeof(weapon) == 12);

struct suspension_animation
{
    i16           mass_point_index;
    animation_ref animation;
    f32           full_extension_ground_depth;
    f32           full_compression_ground_depth;
};
static_assert(sizeof(suspension_animation) == 12);

struct vehicle
{
    screen_bounds                   steering_bounds;
    u32 padding[15];
    enum animation_idx_t
    {
        steering,
        roll,
        throttle,
        velocity,
        braking,
        ground_speed,
        occupied,
        unoccupied,
    };
    reference<animation_ref>        animations;
    reference<suspension_animation> suspension_animations;
};
static_assert(sizeof(vehicle) == 116);

struct device
{
    enum animation_idx_t
    {
        position,
        power,
    };
    reference<animation_ref> animations;
};
static_assert(sizeof(device) == 12);

struct first_person_weapon
{
    u32 padding[4];
    enum animation_idx_t
    {
        idle,
        posing,
        fire_1,
        moving,
        overlays,
        light_off,
        light_on,
        reload_empty,
        reload_full,
        overheated,
        ready,
        put_away,
        overcharged,
        melee,
        fire_2,
        overcharged_jitter,
        throw_grenade,
        ammunition,
        misfire_1,
        misfire_2,
        throw_overheated,
        overheating,
        overheating_again,
        enter,
        exit_empty,
        exit_full,
        o_h_exit, // no idea what o-h-exit means
        o_h_s_enter, // no idea what o-h-s-enter, "oh holy shit"?
    };
    reference<animation_ref> animations;
};
static_assert(sizeof(first_person_weapon) == 28);

struct sound_reference
{
    tagref_typed_t<tag_class_t::snd> sound;
    u32 padding;
};
static_assert(sizeof(sound_reference) == 20);

struct animation
{
    bl_string       name;
    anim_type       type;
    i16             frame_count;
    i16             frame_size; /* bytes per frame */
    frame_info_type frame_info_type_;
    i32             node_list_checksum;
    i16             node_count;
    i16             loop_frame;
    f32             weight;
    i16             key_frame;
    i16             second_key_frame;
    i16             next_animation;
    anim_flags      flags;
    i16             sound_index;
    i16             sound_frame_index;
    i8              left_foot_frame_index;
    i8              right_foot_frame_index;
    animation_ref   main_animation;
    f32             relative_weight;

    data_ref_t frame_info; /* 0x48: root motion delta per frame */

    /* 64-bit bitfields: bit N = node N has this channel */
    u32 node_trans_flags[2];    /* 0x5C: translation */
    u32 unknown_1[2];           /* 0x64 */
    u32 node_rot_flags[2];      /* 0x6C: rotation */
    u32 unknown_2[2];           /* 0x74 */
    u32 node_scale_flags[2];    /* 0x7C: scale */
    u32 unknown_3;              /* 0x84 */
    i32 compressed_data_offset; /* 0x88 */

    data_ref_t default_data; /* 0x8C: default node transforms */
    data_ref_t frame_data;   /* 0xA0: packed per-frame node data */

    bool has_translation(u32 node) const
    {
        if(node >= 64)
            return false;
        return (node_trans_flags[node >> 5] >> (node & 31)) & 1;
    }

    bool has_rotation(u32 node) const
    {
        if(node >= 64)
            return false;
        return (node_rot_flags[node >> 5] >> (node & 31)) & 1;
    }

    bool has_scale(u32 node) const
    {
        if(node >= 64)
            return false;
        return (node_scale_flags[node >> 5] >> (node & 31)) & 1;
    }

    bool is_compressed() const
    {
        return static_cast<u16>(flags) &
               static_cast<u16>(anim_flags::compressed_data);
    }
};
static_assert(sizeof(animation) == 180);
static_assert(offsetof(animation, frame_info) == 72);
static_assert(offsetof(animation, node_trans_flags) == 92);
static_assert(offsetof(animation, node_rot_flags) == 108);
static_assert(offsetof(animation, node_scale_flags) == 124);
static_assert(offsetof(animation, default_data) == 140);
static_assert(offsetof(animation, frame_data) == 160);

enum class header_flags_t : u16
{
    compress_all_animations = 0x1,
    force_idle_compression  = 0x2,
};

struct header
{
    reference<object>              objects;
    reference<unit>                units;
    reference<weapon>              weapons;
    reference<vehicle>             vehicles;
    reference<device>              devices;
    reference<i16>                 unit_damages;
    reference<first_person_weapon> first_person_weapons;
    reference<sound_reference>     sound_refs;
    f32                            limp_body_node_radius; // 0 uses 0.04 default
    header_flags_t                 flags;
    i16                            unknown_;
    reference<node_t>              nodes;
    reference<animation>           animations;
};
static_assert(sizeof(header) == 128);
static_assert(offsetof(header, nodes) == 104);
static_assert(offsetof(header, animations) == 116);

} // namespace blam::antr
