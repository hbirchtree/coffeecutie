#pragma once

#include "blam/volta/blam_tag_classes.h"
#include "blam/volta/blam_tag_ref.h"
#include "blam/volta/hsc/bytecode_common_v12.h"
#include "blam_base_types.h"
#include "blam_bsp_structures.h"
#include "blam_file_header.h"
#include "blam_magic_data.h"
#include "blam_mod2.h"
#include "blam_reference.h"
#include "blam_sound.h"
#include "blam_tag_index.h"
#include "blam_vertex.h"
#include "hsc/blam_bytecode.h"

#include <cstddef>
#include <peripherals/stl/range.h>

namespace blam::scn {

template<typename T>
/*!
 * \brief Encapsulates a base and reference reflexive group
 */
struct reflex_group
{
    reference<T>                       instances;
    reference<std::array<tagref_t, 3>> palette;
};

using angle_t = f32;

/* Data which is not part of the structures proper */

union local_actor_type
{
    i16 data;

    inline operator i16() const
    {
        return data;
    }

    inline bool valid() const
    {
        return data != -1;
    }
};

enum class gamemode_t : u16
{
    none,
    ctf,
    slayer,
    oddball,
    king_of_the_hill,
    race,
    terminator,
    stub,
    ignored1,
    ignored2,
    ignored3,
    ignored4,
    all_games,
    all_except_ctf,
    all_except_race_ctf,
};

using scn_chunk = byte_t[100];

enum class object_type : u16
{
    biped,
    vehicle,
    weapon,
    equipment,
    garbage,
    projectile,
    scenery,
    machine,
    control,
    light_fixture,
    placeholder,
    sound_scenery,
};

enum class object_flags : u16
{
    no_shadow                  = 0x1,
    transparent_self_occlusion = 0x2,
    bright = 0x4, /* "Brighter than it should be", as in unshaded? */
    not_pathfinding_obstacle = 0x8,
};

struct object
{
    object_type                       type;
    object_flags                      flags;
    f32                               bound_radius;
    Vecf3                             bound_offset;
    Vecf3                             origin_offset;
    f32                               acceleration_scale;
    u32                               padding_;
    tagref_typed_t<tag_class_t::mod2> model;
    tagref_typed_t<tag_class_t::antr> anim_graph;
    u32                               padding2[10];
    tagref_typed_t<tag_class_t::coll> collider;
    tagref_typed_t<tag_class_t::pphy> physics;
    tagref_typed_t<tag_class_t::shdr> shader;
    tagref_typed_t<tag_class_t::effe> effect;
    f32                               render_bound_radius;

    struct
    {
        u32 inputs[4];
        i16 hud_msg;
        i16 shader_perm;
    } export_;
};

struct item : object
{
    struct attachment_t
    {
        tagref_t type;
        i16      marker;
        u32      primary_scale;
        u32      second_scale;
        u32      change_color;
    };

    reference<attachment_t> attachments;
};

struct unit : object
{
    struct color_perm_t
    {
        f32   weight;
        Vecf3 lower_bound;
        Vecf3 upper_bound;
    };

    struct change_color_t
    {
        shader::param_src darken_by;
        shader::param_src scale_by;

        enum scale_flags_t : u32
        {
            none         = 0x0,
            blend_in_hsv = 0x1,
            more_hues    = 0x2,
        } scale_flags;

        Vecf3                   lower_bound;
        Vecf3                   upper_bound;
        reference<color_perm_t> permutations;
    };

    u32 padding[39];

    reference<change_color_t> change_colors;
};

struct biped : unit
{
};

struct vehicle : unit
{
};

/* ... Need to figure out a common name for these... */
struct scenery : unit
{
};

struct weapon : item
{
};

struct equipment : item
{
};

struct alignas(4) object_spawn
{
    enum class spawn_flags : u16
    {
        none      = 0x0,
        automatic = 0x1,
        on_easy   = 0x2,
        on_normal = 0x4,
        on_hard   = 0x8,
    };

    enum bsp_flags_t : u16
    {
        none = 0x0,
    };

    i16         ref;
    i16         name;
    spawn_flags flags;
    u16         permutation;
    Vecf3       pos;
    Vecf3       rot;
    bsp_flags_t bsp_flags;
    u16         padding;
};

struct biped_spawn : object_spawn
{
    u32 padding[9];

    enum biped_flags_t : u16
    {
        none = 0x0,
        dead = 0x1,
    };

    f32           vitality;
    biped_flags_t biped_flags;
    u32           padding2[10];
};

static_assert(sizeof(biped_spawn) == 120);

struct vehicle_spawn : object_spawn
{
    u32 unknown_[21];
};

static_assert(sizeof(vehicle_spawn) == 120);

struct equip_spawn : object_spawn
{
    enum equip_flags_t : u16
    {
        none              = 0x0,
        initially_at_rest = 0x1, /* Not affected by gravity */
        obsolete          = 0x2, /* Unused? */
        does_accelerate   = 0x4, /* Not affected by explosions */
    };

    equip_flags_t equip_flags;
    u16           padding;
};

static_assert(sizeof(equip_spawn) == 40);

struct scenery_spawn : object_spawn
{
    u32 unknown_[9];
};

static_assert(sizeof(scenery_spawn) == 72);

struct weapon_spawn : object_spawn
{
    u32 unknown_[14];
};

static_assert(sizeof(weapon_spawn) == 92);

enum class machine_spawn_flags : u16
{
    none              = 0x0,
    initially_on      = 0x1,
    initially_off     = 0x2,
    changes_once      = 0x4,
    position_reversed = 0x8,
    not_usable        = 0x10,
};

struct device_machine_spawn : object_spawn
{
    enum class machine_spawn_flags2 : u16
    {
        none                 = 0x0,
        one_sided            = 0x1,
        never_appears_locked = 0x2,
        opened_by_melee      = 0x4,
    };

    i16                  power_group;
    i16                  position_group;
    machine_spawn_flags  internal_flags;
    machine_spawn_flags2 device_flags;

    u32 padding[5];
};

static_assert(sizeof(device_machine_spawn) == 64);

struct light_fixture_spawn : object_spawn
{
    i16                 power_group;
    i16                 position_group;
    machine_spawn_flags flags;
    Vecf3               color;
    f32                 intensity;
    f32                 falloff_angle;
    f32                 cutoff_angle;

    u32 padding[5];
};

static_assert(offsetof(light_fixture_spawn, power_group) == 36);
static_assert(sizeof(light_fixture_spawn) == 88);

struct palette
{
    tagref_t tag;
    byte_t   unk[32];
};

struct weapon_instance
{
    u32      reserved1[8];
    u32      unknown;
    tagref_t something;
};

using weapon_tagref = reference<weapon_instance>;

// struct scenery
// {
//     u32                               count;
//     string_ref                        name;
//     u32                               padding[8];
//     tagref_typed_t<tag_class_t::mod2> model;
// };

struct device_machine : object
{
};

struct sound_scenery
{
    i16   SoundType;
    i16   unk2;
    u32   unk3;
    Vecf3 pos;
    u32   unk4[5];
};

struct player_starting_location
{
    Vecf3      pos;
    f32        rot;
    u16        team_index;
    u16        bsp_index;
    gamemode_t types[4];
    u32        padding[6];
};

static_assert(sizeof(player_starting_location) == 52);

struct multiplayer_flag
{
    Vecf3  pos;
    f32    yaw;
    u16    index1;
    u16    index2;
    bl_tag tag;
    u32    unk2[31];
};

struct item_permutation
{
    u32      padding_[8];
    f32      weight;
    tagref_t item;
};

struct item_collection
{
    reference<item_permutation> items;
    u32                         spawn_time;
};

struct multiplayer_equipment
{
    enum class equipment_flag : u32
    {
        none     = 0x0,
        levitate = 0x1,
    };

    equipment_flag flags;

    gamemode_t types[4];

    u32 team_idx;
    u32 spawn_time;

    u32 padding[11];

    Vecf3    pos;
    f32      facing;
    tagref_t item;
    u32      unk3[12];
};

struct player_starting_profile
{
    bl_string name;
    f32       health_modifier;
    f32       shield_modifier;
    tagref_t  primary_weapon;
    u16       rounds1_loaded;
    u16       rounds1_total;
    tagref_t  secondary_weapon;
    u16       rounds2_loaded;
    u16       rounds2_total;
    u16       frag_grenades;
    u16       plasma_nades;
    u16       c_nades;
    u16       d_nades;
    u32       padding2[4];
};

static_assert(sizeof(player_starting_profile) == 104);

struct device_group
{
    enum class device_group_flags : u32
    {
        none         = 0x0,
        changes_once = 0x1,
    };

    bl_string          name;
    f32                initial_value;
    device_group_flags flags;
    u32                unk[3];
};

/* "Structure BSP switch trigger volume": while `source` is the active
 * structure BSP and the player enters `trigger_volume`, the engine makes
 * `destination` the active BSP. Decoded from b40.map, where the referenced
 * trigger volumes are named 'bsp <source>,<destination>' and entries come
 * in bidirectional pairs. */
struct bsp_trigger
{
    i16 trigger_volume; /* index into scenario trigger_volumes */
    i16 source;         /* index into scenario structure BSPs (bsp_info) */
    i16 destination;    /* index into scenario structure BSPs (bsp_info) */
    i16 unknown;        /* structured (paired like the volumes, -1 for
                           script-only transitions) but undeciphered */
};

static_assert(sizeof(bsp_trigger) == 8);

struct move_positions
{
    bl_string unk1[32];
    byte_t    unk2[4];
    u32       offset;
    u32       unk[5];
};

struct object_name
{
    bl_string name;
    u32       unknown;
};

struct editor_comment
{
    Vecf3 position;
    bl_string comment;
};

struct trigger_volume
{
    u32       unk;
    bl_string name;
    f32       unk2[9]; /* likely 3×Vecf3 orientation axes */
    Vecf3     position;
    Vecf3     extents; /* volume spans position .. position + extents
                          (verified against b40 'null' and door volumes) */

    inline bool contains(Vecf3 const& point) const
    {
        auto in = [](f32 v, f32 a, f32 b) {
            if(a > b)
            {
                f32 t = a;
                a     = b;
                b     = t;
            }
            return v >= a && v <= b;
        };
        Vecf3 hi = position + extents;
        return in(point.x, position.x, hi.x) && in(point.y, position.y, hi.y) &&
               in(point.z, position.z, hi.z);
    }
};

enum class actor_flags_t : u32
{
    none                     = 0x0,
    see_in_darkness          = 0x1,
    sneak_uncovering_target  = 0x2,
    sneak_uncovering_pursuit = 0x4,
    /* unused */
    shoot_at_target_last_location = 0x10,
    stay_still_crouched           = 0x20,
    crouch_on_not_combat          = 0x40,
    crouch_on_guard               = 0x80,
    /* unused */
    crouch_shooter          = 0x100,
    panic_on_surprise       = 0x200,
    vehicle_with_player     = 0x400,
    fire_before_aim         = 0x800,
    no_strafe               = 0x1000,
    crouch_no_strafe        = 0x2000,
    defensive_crouch_charge = 0x4000,
    stalk                   = 0x8000,
    stalk_freeze_on_exposed = 0x10000,
    berserk_always          = 0x20000,
    berserk_panic_moving    = 0x40000,
    flying                  = 0x80000,
    panic_for_unstoppable   = 0x100000,
    crouch_on_hiding        = 0x200000,
    always_charge_attack    = 0x400000,
    dive_ledges             = 0x800000,
    swarm                   = 0x1000000,
    suicidal_melee          = 0x2000000,
    crouch_no_move          = 0x4000000,
    crouch_no_look          = 0x8000000,
    crouch_on_fire          = 0x10000000,
    avoid_friendly_fire     = 0x20000000,
};

enum class actor_ex_flags_t : u32
{
    none                       = 0x0,
    avoid_enemy_attack_vectors = 0x1,
    stand_to_fire              = 0x2,
    stop_to_fire               = 0x4,
    disallow_vehicle_combat    = 0x8,
    pathfind_ignore_danger     = 0x10,
    panic_in_group             = 0x20,
    no_corpse_shoot            = 0x40,
};

enum class actor_type_t : u32
{
    elite,
    jackal,
    grunt,
    hunter,
    engineer,
    assassin,
    player,
    marine,
    crew,
    combat_form,
    infection_form,
    carrier_form,
    monitor,
    sentinel,
    none,
    mounted_weapon,
};

enum class danger_trigger_t : u16
{
    never,
    visible,
    shooting,
    shooting_near_us,
    damaging_us,
    unused_1,
    unused_2,
    unused_3,
    unused_4,
    unused_5,
};

C_FLAGS(actor_flags_t, u32)

struct actor
{
    actor_flags_t    flags;
    actor_ex_flags_t extra_flags;
    u32              padding[3];
    actor_type_t     type;

    struct perception_t /* perception */
    {
        f32     max_vision_dist;
        angle_t central_vision;
        angle_t max_vision;
        angle_t peripheral_vision;
        f32     peripheral_dist;
        Vecf3   stand_gun_offset;
        Vecf3   crouch_gun_offset;
        f32     hearing_dist;
        f32     projectile_awareness; /* [0,1] */
        f32     vehicle_awareness;    /* [0,1] */
        f32     combat_perception_time;
        f32     guard_perception_time;
    } perception;

    struct movement_t
    {
        f32 dive_into_cover_change;
        f32 emerge_from_cover_chance;
        f32 dive_from_grenade_chance;
        f32 pathfinding_radius;
        f32 glass_ignorance_chance;
        f32 stationary_movement_dist;
        f32 free_flying_sidestep;
        f32 begin_moving_angle;
    } movement;

    struct looking_t
    {
        bl_rotate_2 maximum_aim_deviation;
        bl_rotate_2 maximum_looking_deviation;
        f32         noncombat_look_delta_L;
        f32         noncombat_look_delta_R;
        f32         combat_look_delta_L;
        f32         combat_look_delta_R;
        bl_rotate_2 idle_aiming_range;
        bl_rotate_2 idle_looking_range;
        Vecf2       event_look_time_modifier;
        Vecf2       noncombat_idle_facing;
        Vecf2       noncombat_idle_aiming;
        Vecf2       noncombat_idle_looking;
        Vecf2       guard_idle_facing;
        Vecf2       guard_idle_aiming;
        Vecf2       guard_idle_looking;
        Vecf2       combat_idle_facing;
        Vecf2       combat_idle_aiming;
        Vecf2       combat_idle_looking;
        tagref_t    do_not_use_1;
        tagref_t    do_not_use_2;
    } looking;

    struct unopposable_t
    {
        danger_trigger_t unreachable_danger_trigger;
        danger_trigger_t vehicle_danger_trigger;
        danger_trigger_t player_danger_trigger;
        Vecf2            danger_trigger_time;
        f32              friends_killed_trigger;
        f32              friends_retreating_trigger;
        Vecf2            retreat_time;
    } unopposable;

    struct panic_t
    {
        Vecf2        cowering_time;
        f32          friend_killed_panic_change;
        actor_type_t leader_type;
        f32          leader_killed_panic_change;
        f32          panic_damage_threshold;
        f32          surprise_distance;
    } panic;

    struct defensive_t
    {
    } defensive;

    struct pursuit_t
    {
        Vecf2 uncover_delay_time;
        Vecf2 target_search_time;
        Vecf2 pursuit_position_time;
        f32   num_positions_coord;
        f32   num_positions_normal;
    } pursuit;

    struct berserk_t
    {
    } berserk;

    struct firing_positions_t
    {
        Vecf2 guard_position_time;
        Vecf2 combat_position_time;
        f32 old_position_avoid_dist;
        f32 friend_avoid_dist;
    } firing_positions;

    struct communication_t
    {
        Vecf2 noncombat_idle_speech_time;
        Vecf2 combat_idle_speech_time;
    } communication;
};

enum class actor_variant_flags_t : u32
{
    none                      = 0x0,
    shoot_while_flying        = 0x1,
    interpolate_color_hsv     = 0x2,
    unlimited_grenades        = 0x4,
    moveswitch_stay_w_friends = 0x8,
    active_camo               = 0x10,
    super_active_camo         = 0x20,
    no_ranged_weapons         = 0x40,
    prefer_passenger_seat     = 0x80,
};

enum class actor_movement_t : u32
{
    always_run,
    always_crouch,
    switchable,
};

struct actor_variant
{
    using cls = tag_class_t;

    actor_variant_flags_t                           flags;
    tagref_typed_t<cls::actr>                       actor;
    tagref_typed_t<cls::bipd, cls::unit, cls::vehi> unit;
    tagref_typed_t<cls::actv>                       major;

    u32 padding[6];

    struct movement_t
    {
        actor_movement_t movement;
        f32              crouch_chance;
        Vecf2            crouch_time;
        Vecf2            run_time;
    } movement;

    struct combat_t
    {
        tagref_typed_t<cls::weap> weapon;

        f32     max_fire_dist;
        f32     rate_of_fire;
        angle_t projectile_error;
        Vecf2   burst_delay;
        f32     retarget_fire_time;

        f32 surprise_delay;
        f32 surprise_wild_fire_time;

        f32 death_wild_fire_chance;
        f32 death_wild_fire_time;

        Vecf2 desired_combat_range;
        Vecf3 custom_stand_gun_offset;
        Vecf3 custom_crouch_gun_offset;

        Vecf2 target_track;
        Vecf2 target_lead;
        f32   dmg_modifier;
        f32   dmg_per_second;
    } combat;
};

using actor_variant_ref = tagref_t;

struct antenna
{
    bl_string                         attachment_marker_name;
    tagref_typed_t<tag_class_t::bitm> bitmaps;
    tagref_typed_t<tag_class_t::pphy> physics;
    f32                               spring_strength_coeff;
    f32                               falloff_pixels;
    f32                               cutoff_pixels;

    struct vertex
    {
        f32   spring_strength_coeff;
        f32   angle_yaw, angle_pitch;
        f32   length;
        i16   sequence_index;
        i16   padding;
        Vecf4 color;
        Vecf4 lod_color;
    };

    reference<vertex> vertices;
};

namespace ai {

enum class state_t : i16
{
    unknown_1 = 0,
    unknown_2 = 2,
    unknown_3 = 6,
    unknown_4 = 7,
    unknown_5 = 9,
    unknown_6 = 10,
    unknown_7 = 11,
};

struct animation_ref
{
    bl_string name;
    tagref_typed_t<tag_class_t::antr> animation_graph;
    u32       unk[3];
};

static_assert(sizeof(animation_ref) == 60);

struct script_ref
{
    bl_string_var<40> name;
};

static_assert(sizeof(script_ref) == 40);

struct recording_ref
{
    bl_string_var<40> script_name;
};

static_assert(sizeof(recording_ref) == 40);

struct platoon
{
    bl_string name;

    enum flags_t : u16
    {
        none,
        flee_when_maneuvering          = 0x1,
        say_advancing_when_maneuvering = 0x2,
        start_in_defending_state       = 0x4,
    } flags;

    struct cause_t
    {
        i16 target;

        enum event_t : u16
        {
            never,
            under_75_str,
            under_50_str,
            under_25_str,
            any_dead,
            dead_25,
            dead_50,
            dead_75,
            one_alive,
            all_dead,
        } event;
    };

    cause_t change_attacking_defending;
    cause_t maneuver_when;

    u32 padding[32];
};

struct firing_position
{
    Vecf3 position;
    u16   unknown1;
    u16   unknown_idx;
    u32   unknown[2];
};

struct squad
{
    bl_string         name;
    local_actor_type  actor_type;
    u16               platoon;
    state_t           initial_state;
    state_t           return_state;
    u32               unk1[11];
    u32               attacking;
    u32               attacking_search;
    u32               attacking_guard;
    u32               defending;
    u32               defending_search;
    u32               defending_guard;
    u32               pursuing;
    u32               unk2[3];
    u16               normal_diff_count;
    u16               insane_diff_count;
    u32               unk3[20];
    reference<byte_t> start_locations;
    u32               unk4[3];
};

struct squad_spawn
{
    Vecf3      position;
    f32        yaw;
    u16        team_index;
    u16        bsp_index;
    gamemode_t types[4];
};

static_assert(sizeof(squad_spawn) == 28);

struct encounter
{
    bl_string_var<16>          name;
    enum flags_t : u16
    {
        none                       = 0x0,
        not_initially_created      = 0x1,
        respawn_enabled            = 0x2,
        initially_blind            = 0x4,
        initially_deaf             = 0x8,
        initially_braindead        = 0x10,
        _3d_firing_positions       = 0x20,
        manual_bsp_index_specified = 0x40,
    } flags;
    enum team_index_t : u16
    {
        default_by_unit,
        player,
        human,
        covenant,
        flood,
        sentinel,
        unused_6,
        unused_7,
        unused_8,
        unused_9,
    } team_index;
    enum search_behavior_t : u16
    {
        search_normal,
        search_never,
        search_tenacious,
    } search_behavior;
    i16 manual_bsp_index;
    Vecf2 respawn_delay;
    u32                        unk[24];
    reference<squad>           squads;
    reference<platoon>         platoons;
    reference<firing_position> firing_positions;
    reference<squad_spawn>     start_locations;
};

static_assert(sizeof(encounter) == 176);

struct command_list_command
{
    enum atom_type_t : u16
    {
        pause,
        go_to,
        goto_and_face,
        move_in_direction,
        look,
        animation_mode,
        crouch,
        shoot,
        grenade,
        vehicle,
        running_jump,
        targeted_jump,
        script_,
        animate,
        recording_,
        action,
        vocalize,
        targeting,
        initiative,
        wait,
        loop,
        die,
        move_immediate,
        look_random,
        look_player,
        look_object,
        set_radius,
        teleport,
    } atom_type;
    u16 atom_modifier;
    u16 param1;
    u16 param2;
    i16 point_1;     // pointing to points list
    i16 point_2;     // pointing to points list
    i16 animation;   // pointing to ai animation ref list
    i16 script;      // pointing to ai script ref list
    i16 recording;   // pointing to ai recording ref list
    i16 command;     // pointing into commands list
    i16 object_name; // points to object name list
};

struct command_list_point
{
    Vecf3 position;
};

struct command_list
{
    bl_string name;
    enum flags_t : u16
    {
        none                = 0x0,
        allow_initiative    = 0x1,
        allow_targeting     = 0x2,
        disable_looking     = 0x4,
        disable_comms       = 0x8,
        disable_fall_damage = 0x10,
        manual_bsp_index_    = 0x20,
    } flags;
    i16                             manual_bsp_index;
    reference<command_list_command> commands;
    reference<command_list_point>   points;
};

struct conversation_participant
{
    enum flags_t : u16
    {
        none           = 0x0,
        is_optional    = 0x1,
        has_alternate  = 0x2,
        is_alternative = 0x4,
    } flags;
    enum selection_type_t : u16
    {
        friendly_actor,
        disembodied,
        in_players_vehicle,
        not_in_a_vehicle,
        prefer_sergeant,
        any_actor,
        radio_unit,
        radio_sergeant,
    } selection_type;
    actor_type_t actor_type;
    i16          use_this_object; // references object_names list
    i16          set_new_name;    // references object_names list
    u32 padding[10];
    bl_string    encounter_name;
};

static_assert(sizeof(conversation_participant) == 84);

struct conversation_line
{
    enum flags_t : u16
    {
        addressee_looking_at_speaker     = 0x1,
        everyone_look_at_speaker         = 0x2,
        everyone_look_at_addressee       = 0x4,
        wait_after_until_told_to_advance = 0x8,
        wait_until_speaker_nearby        = 0x10,
        wait_until_everyone_nearby       = 0x20,
    } flags;
    i16                              participant;
    i16                              addressee;
    i16                              addressee_participant;
    f32                              line_delay_time;
    tagref_typed_t<tag_class_t::snd> variants[6];

    u32 padding[4];
};

static_assert(sizeof(conversation_line) == 124);

struct conversation
{
    bl_string name;
    enum conversation_flags_t : u16
    {
        none                     = 0x0,
        stop_if_death            = 0x1,
        stop_if_damaged          = 0x2,
        stop_if_visible_enemy    = 0x4,
        stop_if_alerted_to_enemy = 0x8,
        player_must_be_visible   = 0x10,
        stop_other_actions       = 0x20,
        keep_trying_to_play      = 0x40,
        player_must_be_looking   = 0x80,
    } flags;
    f32                                 trigger_distance;
    f32                                 run_to_player_dist;
    u32 padding_[9];
    reference<conversation_participant> participants;
    reference<conversation_line>        lines;

    u32 padding[3];
};

static_assert(sizeof(conversation) == 116);

} // namespace ai

struct script_trigger
{
    u32       padding_1;
    bl_string name;
    Vecf3     pos;
    u32       some_value;
    u32       padding[10];
};

struct references
{
    u32       unk1;
    bl_string name;
    u32       unk[15];
};

struct decal
{
    i16   unk1;
    i16   unk2;
    Vecf3 pos;
};

struct decal_ref
{
    tagref_typed_t<tag_class_t::deca> reference;
};

struct detail_object_collection_ref
{
    tagref_typed_t<tag_class_t::dobc> reference;
};

struct shader_index
{
    u32 ShaderType;
    u32 ShaderIndex;
};

using skybox_ref = tagref_t;

struct skybox
{
    struct shader_function
    {
        bl_tag global_func;
    };

    struct animation
    {
        i16 anim_idx;
        u32 period;
    };

    struct light
    {
        enum class light_flags : u32
        {
            none              = 0x0,
            affects_exteriors = 0x1,
            affects_interiors = 0x2,
        };

        tagref_typed_t<tag_class_t::lens> lens_flare;
        i8                                marker_id;
        bl_string                         marker_name;

        u32 padding_2[6];

        struct radiosity_t
        {
            light_flags flags;
            Vecf3       color;
            f32         power;
            f32         test_distance;
            f32         unknown;
            Vecf2       direction; /* yaw, pitch */
            f32         diameter;
        } radiosity;
    };

    struct radiosity_opts
    {
        Vecf3 color;
        f32   power;
    };

    struct fog_opts
    {
        Vecf3 color;
        u32   padding[2];
        f32   density;
        f32   start_distance;
        f32   opaque_distance;
    };

    tagref_typed_t<tag_class_t::mod2> model;
    tagref_typed_t<tag_class_t::antr> anim_graph;

    u32 padding_1[6];

    radiosity_opts indoor_ambient;
    radiosity_opts outdoor_ambient;
    fog_opts       outdoor_fog;
    fog_opts       indoor_fog;
    tagref_t       indoor_fog_screen;

    u32 padding_3[1];

    reference<shader_function> shader_functions;
    reference<animation>       animations;
    reference<light>           lights;
}; // namespace scn

static_assert(sizeof(skybox) == 208);
static_assert(sizeof(skybox::light) == 116);
static_assert(offsetof(skybox, indoor_ambient) == 56);
static_assert(offsetof(skybox, lights) == 196);
static_assert(offsetof(skybox::light, radiosity) == 76);

struct starting_equip
{
    u32                     unknown1; /*!< Sometimes 1? */
    u32                     index;
    byte_t                  padding2[51];
    std::array<tagref_t, 6> items;
    byte_t                  padding3[45];
};

struct control : object_spawn
{
    u32 padding[7];
};

static_assert(sizeof(control) == 64);

struct light_fixture
{
    bl_rgba_t ambient;
    byte_t    unk1[4];
    Vecf3     pos;
    byte_t    unk2[12];
    bl_rgba_t specular;
    i32       zero1;
    bl_rgba_t color;
    byte_t    unk3[40];
};

struct cutscene_flag
{
    u32       garbage;
    bl_string name;
    Vecf3     position;
    Vecf2     facing;
    u32       padding[9];
};

static_assert(sizeof(cutscene_flag) == 92);

struct cutscene_camera_position
{
    u32               padding; // no idea why there's garbage here
    bl_string_var<36> name;
    Vecf3             position;
    Vecf3             rotation; // components in radians, editor shows degrees!
    f32               fov; // same as above

    u32 padding_[9];

    inline std::pair<Vecf3, Quatf> to_camera(
        typing::vector_types::Matf3 const& basis) const
    {
        return {
            position,
            glm::normalize(
                glm::angleAxis(rotation.y, basis[1]) *
                glm::angleAxis(
                    rotation.x + glm::pi<f32>(), basis[0]) *
                glm::angleAxis(rotation.z, basis[2])),
        };
    }
};

static_assert(sizeof(cutscene_camera_position) == 104);

struct cutscene_title
{
    u32               garbage;
    bl_string_var<36> name;
    bl_rect           text_bounds;
    i16               string_index;
    enum justification_t
    {
        left,
        right,
        center,
    } justification;
    bl_rgba_t text_color;
    bl_rgba_t shadow_color;
    u32       fade_time;
    u32       up_time;
    u32       fade_out_time;

    u32 padding_[5];
};

static_assert(sizeof(cutscene_title) == 96);

/*!
 * \brief A Blam! scenario descriptor
 */
template<typename V>
requires is_game_version<V>
struct scenario
{
    enum class scenario_type : u16
    {
        solo,
        multiplayer,
        main_menu,
    };
    enum class scenario_flags : u16
    {
        none         = 0x0,
        cortana_hack = 0x1,
        demo_ui      = 0x2,
    };

    using bytecode_t = typename V::bytecode_type;

    struct scenario_info_t /* 260-byte block? */
    {
        tagref_t unk_bsp1; // Unused
        tagref_t unk_bsp2; // Unused
        tagref_t unk_sky;  // Unused

        reference<skybox_ref> skyboxes;

        scenario_type  type;
        scenario_flags flags;

        reference<tagref_t> child_scenarios;

        f32 local_north;

        reference<u32> predicted_resource;
        reference<u32> functions;

        u32 padding1[39];
    } info;

    struct editor_t /* 256-byte block */
    {
        i32                       scenario_size;
        u32                       unknown_2;
        reference<editor_comment> comments;
        u32                       padding2[59];
    } editor;

    struct objects_t /* 324-byte block, object spawns */
    {
        reference<object_name>             object_names;
        reflex_group<scenery_spawn>        scenery;
        reflex_group<biped_spawn>          bipeds;
        reflex_group<vehicle_spawn>        vehicles;
        reflex_group<equip_spawn>          equips;
        reflex_group<weapon_spawn>         weapon_spawns;
        reference<device_group>            device_groups;
        reflex_group<device_machine_spawn> machines;
        reflex_group<control>              controls;
        reflex_group<light_fixture_spawn>  light_fixtures;
        reflex_group<sound_scenery>        snd_scenery;

        u32 padding3[21];
    } objects;

    struct player_start_t
    {
        reference<player_starting_profile>  profiles;
        reference<player_starting_location> locations;
    } player_start;

    reference<trigger_volume> trigger_volumes;
    reference<scn_chunk>      recorded_animations;

    struct multiplayer_t /* 216-byte block */
    {
        reference<multiplayer_flag>      flags;
        reference<multiplayer_equipment> equipment;
    } netgame;

    reference<starting_equip>               starting_equipment;
    reference<bsp_trigger>                  bsp_switch_triggers;
    reflex_group<decal>                     decals;
    reference<detail_object_collection_ref> detail_object_collection;
    u32                                     padding4[21];

    struct ai_info_t /* 340-byte block */
    {
        reference<actor_variant_ref> actor_palette;
        reference<ai::encounter>     encounters;
        reference<ai::command_list>  command_lists;
        reference<ai::animation_ref> animation_references;
        reference<ai::script_ref>    script_references;
        reference<ai::recording_ref> recording_references;
        reference<ai::conversation>  conversations;
    } ai;

    struct script_t
    {
        u32                                    script_bytecode_size;
        u32                                    unknown_7;
        reference<hsc::script_ref<bytecode_t>> unknown_9;
        u32                                    script_function_table_offset;
        u32                                    script_function_table_size;
        reference<char>                        script_string_segment;
        reference<hsc::function_declaration>   scripts;
        reference<hsc::global>                 globals;
        reference<scn_chunk> references; /* references to what? */
        reference<scn_chunk> unknown1;
        reference<scn_chunk> unknown2;
        reference<scn_chunk> unknown3;
    } script;

    struct cutscene_t
    {
        reference<cutscene_flag>            flags;
        reference<cutscene_camera_position> camera_points;
        reference<cutscene_title>           titles;
    } cutscene;

    reference<scn_chunk> unknown7;
    reference<scn_chunk> unknown8;
    reference<scn_chunk> unknown9;
    reference<scn_chunk> unknown10;
    u32                  padding5[15];

    inline Span<hsc::opcode_layout<bytecode_t> const> bytecode(
        map_ptr const& magic) const
    {
        reference<hsc::opcode_layout<bytecode_t>> data{
            .count = static_cast<u32>(
                (script.script_bytecode_size -
                 sizeof(hsc::script_ref<bytecode_t>)) /
                sizeof(hsc::opcode_layout<bytecode_t>)),
            .offset = script.unknown_9.offset + 36,
        };
        return data.data(magic).value();
    }

    inline result<string_segment_ref, error_msg> string_segment(
        map_ptr const& magic) const
    {
        if(auto string_base_chunk = script.script_string_segment.data(magic);
           string_base_chunk.has_error())
            return string_base_chunk.error();
        else if(auto chunk = string_base_chunk.value(); true)
        {
            return string_segment_ref{
                .data = std::string_view(chunk.data(), chunk.size()),
            };
        }
    }

    inline Span<hsc::function_declaration const> function_table(
        map_ptr const& magic) const
    {
        return script.scripts.data(magic).value();
    }

    struct
    {
        tagref_t custom_object_names; /*!< Points to ui::unicode_ref */
        tagref_t cutscene_titles;     /*!< Points to ui::unicode_ref*/
        tagref_t hud_text;            /*!< Points to ui::hud_message */
    } ui_text;

    reference<bsp::info> bsp_info;
};

static_assert(sizeof(scenario<xbox_version_t>) == 1456);
static_assert(sizeof(scenario<pc_version_t>) == 1456);

// Add these checks so that, if something is moved or removed
// We have some reference points to find out where
static_assert(offsetof(scenario<pc_version_t>, objects) == 516);
static_assert(offsetof(scenario<pc_version_t>, ai) == 1056);
static_assert(offsetof(scenario<pc_version_t>, cutscene) == 1252);

template<typename V>
requires is_game_version<V>
inline scenario<V> const& get_scenario(
    file_header_t const* header, map_ptr const& magic)
{
    return tag_index_t<V>::from_header(header)
        .scenario(header)
        .template to_reflexive<scenario<V>>()
        .data(magic)[0];
}

} // namespace blam::scn

namespace blam::ui {

struct unicode_ref
{
    u32                       length;
    u32                       padding;
    reference<unicode_var<1>> data;

    inline result<ucs_string, error_msg> str(
        map_ptr const& magic, u16 off = 0) const
    {
        if(auto str_data = data.data(magic); str_data.has_error())
            return str_data.error();
        else
            return str_data.value()[0].str(off);
    }
};

struct unicode_string
{
    reference<unicode_ref> sub_strings;
};

struct hud_symbol
{
    bl_string symbol;
    u16       offset;
    u16       unknown;
    u32       padding[7];
};

struct hud_message
{
    struct offset_pair
    {
        /* Could be a big-endian u16? */
        u8 pad;
        u8 size;
    };

    unicode_ref            text;
    reference<offset_pair> offsets;
    reference<hud_symbol>  symbols;

    /* Get offset into unicode_ref for a string */
    inline std::optional<u16> str_offset(map_ptr const& magic, u32 idx) const
    {
        if(auto offset_data = offsets.data(magic); offset_data.has_error())
            return std::nullopt;
        else
        {
            u16 out = 0;
            for(auto i : stl_types::Range<>(idx))
            {
                out += offset_data.value()[i].size;
            }
            return out;
        }
    }

    inline std::optional<ucs_string> symbol_find(
        map_ptr const& magic, std::string_view const& sym) const
    {
        auto symbol_res = symbols.data(magic);
        if(symbol_res.has_error())
            return std::nullopt;
        auto symbol_data = std::move(symbol_res.value());
        for(hud_symbol const& s : symbol_data)
        {
            if(s.symbol != sym)
                continue;

            if(auto symbol = text.data.data(magic); symbol.has_error())
                continue;
            else
                return symbol.value()[0].view(text.length, s.offset);
        }

        return std::nullopt;
    }
};

} // namespace blam::ui
