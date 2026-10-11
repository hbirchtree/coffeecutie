#pragma once

#include "objects.h"
#include "structures.h"

#include <array>

namespace blam::dimeter::scn {
struct palette_entry_t;
struct unit_placement_t;
struct object_name_t;
struct trigger_volume_t;
} // namespace blam::dimeter::scn

namespace blam::dimeter::hsc {
struct script_t;
} // namespace blam::dimeter::hsc

namespace blam::dimeter::geo {
struct bsp_cluster_t;
struct structure_bsp_ref_t;
} // namespace blam::dimeter::geo

namespace blam::dimeter::ai {

struct squad_group_t;
struct squad_t;
struct zone_t;
struct area_t;
struct order_t;
struct trigger_t;
struct character_ref_t;
struct style_ref_t;

/*!
 * Halo 2 AI as laid out in the scenario: squad groups and squads with
 * their spawn points, zones holding firing positions and areas, orders
 * and the triggers that advance them, mission scenes and the character
 * palette. Layout from Assembly's Halo2Xbox scnr.xml.
 */

using team_t = obj::team_t;

enum class grenade_type_t : u16
{
    none = 0,
    human,
    covenant_plasma,
};

enum class seat_type_t : u16
{
    default_ = 0,
    passenger,
    gunner,
    driver,
    small_cargo,
    large_cargo,
    no_driver,
    no_vehicle,
};

enum class movement_mode_t : u16
{
    default_ = 0,
    climbing,
    flying,
};

enum class major_upgrade_t : u16
{
    normal = 0,
    few,
    many,
    none,
    all,
};

/*! How a set of triggers combines: any of them, or all of them */
enum class combination_rule_t : u16
{
    any = 0,
    all,
};

enum class dialogue_type_t : u16
{
    none = 0,
    advance,
    charge,
    fall_back,
    retreat,
    move_one,
    arrival,
    enter_vehicle,
    exit_vehicle,
    follow_player,
    leave_player,
    support,
};

enum class combat_status_t : u16
{
    none = 0,
    asleep,
    idle,
    alert,
    combat,
};

enum class area_type_t : u16
{
    default_ = 0,
    search,
    goal,
};

enum class addressee_t : u16
{
    none = 0,
    player,
    participant,
};

enum class role_group_t : u16
{
    group_1 = 0,
    group_2,
    group_3,
};

enum class squad_location_flags_t : u32
{
    none                   = 0x0,
    initially_asleep       = 0x1,
    infection_form_explode = 0x2,
    always_place           = 0x8,
    initially_hidden       = 0x10,
};
C_FLAGS(squad_location_flags_t, u32);

enum class squad_flags_t : u32
{
    none                          = 0x0,
    never_search                  = 0x2,
    start_timer_immediately       = 0x4,
    no_timer_delay_forever        = 0x8,
    magic_sight_after_timer       = 0x10,
    automatic_migration           = 0x20,
    respawn_enabled               = 0x80,
    blind                         = 0x100,
    deaf                          = 0x200,
    braindead                     = 0x400,
    firing_positions_3d           = 0x800,
    initially_placed              = 0x1000,
    units_not_enterable_by_player = 0x2000,
};
C_FLAGS(squad_flags_t, u32);

enum class firing_position_flags_t : u16
{
    none                = 0x0,
    open                = 0x1,
    partial             = 0x2,
    closed              = 0x4,
    mobile              = 0x8,
    wall_lean           = 0x10,
    perch               = 0x20,
    ground_point        = 0x40,
    dynamic_cover_point = 0x80,
};
C_FLAGS(firing_position_flags_t, u16);

enum class area_flags_t : u32
{
    none                   = 0x0,
    vehicle_area           = 0x1,
    perch                  = 0x2,
    manual_reference_frame = 0x4,
};
C_FLAGS(area_flags_t, u32);

enum class zone_flags_t : u32
{
    none             = 0x0,
    manual_bsp_index = 0x1,
};
C_FLAGS(zone_flags_t, u32);

/*! On a trigger reference or rule: the condition is inverted */
enum class trigger_ref_flags_t : u32
{
    none   = 0x0,
    negate = 0x1,
};
C_FLAGS(trigger_ref_flags_t, u32);

enum class mission_scene_flags_t : u32
{
    none                    = 0x0,
    can_play_multiple_times = 0x1,
    enable_combat_dialogue  = 0x2,
};
C_FLAGS(mission_scene_flags_t, u32);

enum class conversation_line_flags_t : u16
{
    none                             = 0x0,
    addressee_look_at_speaker        = 0x1,
    everyone_look_at_speaker         = 0x2,
    everyone_look_at_addressee       = 0x4,
    wait_after_until_told_to_advance = 0x8,
    wait_until_speaker_nearby        = 0x10,
    wait_until_everyone_nearby       = 0x20,
};
C_FLAGS(conversation_line_flags_t, u16);

enum class conversation_flags_t : u16
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
};
C_FLAGS(conversation_flags_t, u16);

enum class special_movement_flags_t : u32
{
    none      = 0x0,
    jump      = 0x1,
    climb     = 0x2,
    vault     = 0x4,
    mount     = 0x8,
    hoist     = 0x10,
    wall_jump = 0x20,
};
C_FLAGS(special_movement_flags_t, u32);

enum class order_flags_t : u32
{
    none                          = 0x0,
    locked                        = 0x1,
    always_active                 = 0x2,
    debug_on                      = 0x4,
    strict_area_def               = 0x8,
    follow_closest_player         = 0x10,
    follow_squad                  = 0x20,
    active_camo                   = 0x40,
    suppress_combat_until_engaged = 0x80,
    inhibit_vehicle_use           = 0x100,
};
C_FLAGS(order_flags_t, u32);

enum class trigger_flags_t : u32
{
    none                    = 0x0,
    latch_on_when_triggered = 0x1,
};
C_FLAGS(trigger_flags_t, u32);

struct alignas(4) squad_group_t
{
    bl_string                  name;
    block_index<squad_group_t> parent;
    block_index<order_t>       initial_order;
};

static_assert(sizeof(squad_group_t) == 0x24);

/*! Where one actor of a squad spawns */
struct alignas(4) squad_location_t
{
    string_id                          name;
    Vecf3                              position;
    i16                                reference_frame_raw;
    i16                                unknown_0;
    Vecf2                              facing; /*!< yaw, pitch in degrees */
    squad_location_flags_t             flags;
    block_index<character_ref_t>       character_type;
    block_index<scn::palette_entry_t>  initial_weapon; /*!< weapon palette */
    block_index<scn::palette_entry_t>  initial_secondary_weapon;
    i16                                unknown_1;
    block_index<scn::palette_entry_t>  vehicle_type; /*!< vehicle palette */
    u16                                seat_type_raw;
    u16                                grenade_type_raw;
    i16                                swarm_count_raw;
    string_id                          actor_variant;
    string_id                          vehicle_variant;
    f32                                initial_movement_distance;
    block_index<scn::unit_placement_t> emitter_vehicle;
    movement_mode_t                    initial_movement_mode;
    bl_string                          placement_script;
    block_index<hsc::script_t>         placement_script_index;
    i16                                unknown_2;

    inline seat_type_t seat_type() const
    {
        return static_cast<seat_type_t>(from_le(seat_type_raw));
    }
};

static_assert(offsetof(squad_location_t, placement_script) == 0x40);
static_assert(sizeof(squad_location_t) == 0x64);

struct alignas(4) squad_t
{
    bl_string                         name;
    squad_flags_t                     flags;
    u16                               team_raw;
    block_index<squad_group_t>        parent_group;
    f32                               delay_time;
    i16                               normal_difficulty_count_raw;
    i16                               insane_difficulty_count_raw;
    major_upgrade_t                   major_upgrade;
    i16                               unknown_0;
    block_index<scn::palette_entry_t> vehicle_type;
    block_index<character_ref_t>      character_type;
    block_index<zone_t>               initial_zone;
    i16                               unknown_1;
    block_index<scn::palette_entry_t> initial_weapon;
    block_index<scn::palette_entry_t> initial_secondary_weapon;
    u16                               grenade_type_raw;
    block_index<order_t>              initial_order;
    string_id                         vehicle_variant;
    tag_block<squad_location_t>       starting_locations;
    bl_string                         placement_script;
    block_index<hsc::script_t>        placement_script_index;
    i16                               unknown_2;

    inline team_t team() const
    {
        return static_cast<team_t>(from_le(team_raw));
    }

    inline i16 normal_difficulty_count() const
    {
        return from_le(normal_difficulty_count_raw);
    }

    inline i16 insane_difficulty_count() const
    {
        return from_le(insane_difficulty_count_raw);
    }
};

static_assert(offsetof(squad_t, starting_locations) == 0x48);
static_assert(sizeof(squad_t) == 0x74);

struct alignas(4) firing_position_t
{
    Vecf3                           position;
    i16                             reference_frame_raw;
    firing_position_flags_t         flags;
    block_index<area_t>             area;
    block_index<geo::bsp_cluster_t> cluster;
    i16                             unknown_0[2];
    Vecf2                           normal; /*!< yaw, pitch in degrees */
};

static_assert(sizeof(firing_position_t) == 0x20);

struct alignas(2) flight_hint_ref_t
{
    i16 flight_hint_raw;
    i16 point_raw;
};

static_assert(sizeof(flight_hint_ref_t) == 4);

struct alignas(4) area_t
{
    bl_string                      name;
    area_flags_t                   flags;
    Vecf3                          position;
    u8                             unknown_0[0x38 - 0x30];
    block_index<firing_position_t> firing_position_start;
    i16                            firing_position_count_raw;
    u8                             unknown_1[0x7C - 0x3C];
    i16                            manual_reference_frame_raw;
    i16                            unknown_2;
    tag_block<flight_hint_ref_t>   flight_hints;

    inline i16 firing_position_count() const
    {
        return from_le(firing_position_count_raw);
    }
};

static_assert(offsetof(area_t, flight_hints) == 0x80);
static_assert(sizeof(area_t) == 0x88);

struct alignas(4) zone_t
{
    bl_string                             name;
    zone_flags_t                          flags;
    block_index<geo::structure_bsp_ref_t> manual_bsp;
    i16                                   unknown_0;
    tag_block<firing_position_t>          firing_positions;
    tag_block<area_t>                     areas;
};

static_assert(sizeof(zone_t) == 0x38);

/*! Reference to an AI trigger from an order, scene or mission */
struct alignas(4) trigger_ref_t
{
    trigger_ref_flags_t    flags;
    block_index<trigger_t> trigger;
    i16                    unknown_0;
};

static_assert(sizeof(trigger_ref_t) == 8);

struct alignas(4) trigger_condition_t
{
    combination_rule_t       combination_rule;
    i16                      unknown_0;
    tag_block<trigger_ref_t> triggers;
};

static_assert(sizeof(trigger_condition_t) == 0xC);

struct alignas(4) role_variant_t
{
    string_id designator;
};

struct alignas(4) role_t
{
    string_id                 name;
    role_group_t              group;
    i16                       unknown_0;
    tag_block<role_variant_t> variants;
};

static_assert(sizeof(role_t) == 0x10);

struct alignas(4) mission_scene_t
{
    string_id                      name;
    mission_scene_flags_t          flags;
    tag_block<trigger_condition_t> trigger_conditions;
    tag_block<role_t>              roles;
};

static_assert(sizeof(mission_scene_t) == 0x18);

struct alignas(4) character_ref_t
{
    tag_reference<tag_class_t::char_> character;
};

static_assert(sizeof(character_ref_t) == 8);

struct alignas(4) animation_reference_t
{
    bl_string                        name;
    tag_reference<tag_class_t::jmad> graph;
    u8                               unknown_0[0x34 - 0x28];
};

static_assert(sizeof(animation_reference_t) == 0x34);

struct alignas(4) named_reference_t
{
    bl_string name;
    u8        unknown_0[8];
};

static_assert(sizeof(named_reference_t) == 0x28);

struct alignas(4) conversation_participant_t
{
    u8                              unknown_0[8];
    block_index<scn::object_name_t> use_object_name;
    i16                             set_new_name_raw;
    u8                              unknown_1[0x24 - 0xC];
    bl_string                       encounter_name;
    u8                              unknown_2[0x54 - 0x44];
};

static_assert(sizeof(conversation_participant_t) == 0x54);

struct alignas(4) conversation_line_t
{
    conversation_line_flags_t                      flags;
    block_index<conversation_participant_t>        participant;
    addressee_t                                    addressee;
    block_index<conversation_participant_t>        addressee_participant;
    u8                                             unknown_0[4];
    f32                                            line_delay_time;
    u8                                             unknown_1[0x1C - 0x10];
    std::array<tag_reference<tag_class_t::snd>, 6> variants;
};

static_assert(sizeof(conversation_line_t) == 0x4C);

struct alignas(4) conversation_t
{
    bl_string                             name;
    conversation_flags_t                  flags;
    i16                                   unknown_0;
    f32                                   trigger_distance;
    f32                                   run_to_player_distance;
    u8                                    unknown_1[0x50 - 0x2C];
    tag_block<conversation_participant_t> participants;
    tag_block<conversation_line_t>        lines;
    tag_block<u8>                         unknown_2;
};

static_assert(sizeof(conversation_t) == 0x68);

/*! A zone's area an order sends its squad to */
struct alignas(4) area_ref_t
{
    area_type_t         area_type;
    i16                 unknown_0;
    block_index<zone_t> zone;
    block_index<area_t> area; /*!< of that zone */
};

static_assert(sizeof(area_ref_t) == 8);

struct alignas(4) secondary_set_trigger_t
{
    combination_rule_t       combination_rule;
    dialogue_type_t          dialogue_type;
    tag_block<trigger_ref_t> triggers;
};

static_assert(sizeof(secondary_set_trigger_t) == 0xC);

struct alignas(4) special_movement_t
{
    special_movement_flags_t flags;
};

struct alignas(4) order_ending_t
{
    block_index<order_t>     next_order;
    combination_rule_t       combination_rule;
    f32                      delay_time;
    dialogue_type_t          dialogue_type;
    i16                      unknown_0;
    tag_block<trigger_ref_t> triggers;
};

static_assert(sizeof(order_ending_t) == 0x14);

struct alignas(4) order_t
{
    bl_string                          name;
    block_index<style_ref_t>           style;
    i16                                unknown_0;
    order_flags_t                      flags;
    combat_status_t                    force_combat_status;
    i16                                unknown_1;
    bl_string                          entry_script;
    block_index<hsc::script_t>         script_index;
    block_index<squad_t>               follow_squad;
    f32                                follow_radius;
    tag_block<area_ref_t>              primary_area_set;
    tag_block<area_ref_t>              secondary_area_set;
    tag_block<secondary_set_trigger_t> secondary_set_trigger;
    tag_block<special_movement_t>      special_movement;
    tag_block<order_ending_t>          endings;
};

static_assert(offsetof(order_t, primary_area_set) == 0x54);
static_assert(sizeof(order_t) == 0x7C);

enum class rule_type_t : u16
{
    a_or_greater_alive = 0,
    a_or_fewer_alive,
    x_or_greater_strength,
    x_or_less_strength,
    if_enemy_sighted,
    after_a_ticks,
    if_alerted_by_squad_a,
    script_reference_true,
    script_reference_false,
    if_player_in_trigger_volume,
    if_all_players_in_trigger_volume,
    combat_status_a_or_more,
    combat_status_a_or_less,
    arrived,
    in_vehicle,
    sighted_player,
    a_or_greater_fighting,
    a_or_fewer_fighting,
    player_within_x_world_units,
    player_shot_more_than_x_seconds_ago,
    game_safe_to_save,
};

struct alignas(4) trigger_rule_t
{
    u16                                rule_type_raw;
    block_index<squad_t>               squad;
    block_index<squad_group_t>         squad_group;
    i16                                a_raw;
    f32                                x;
    block_index<scn::trigger_volume_t> trigger_volume;
    i16                                unknown_0;
    bl_string                          exit_condition_script;
    block_index<hsc::script_t>         exit_condition_script_index;
    i16                                unknown_1;
    trigger_ref_flags_t                flags;

    inline rule_type_t rule_type() const
    {
        return static_cast<rule_type_t>(from_le(rule_type_raw));
    }

    inline i16 a() const
    {
        return from_le(a_raw);
    }
};

static_assert(sizeof(trigger_rule_t) == 0x38);

struct alignas(4) trigger_t
{
    bl_string                 name;
    trigger_flags_t           flags;
    combination_rule_t        combination_rule;
    i16                       unknown_0;
    tag_block<trigger_rule_t> conditions;
};

static_assert(sizeof(trigger_t) == 0x30);

struct alignas(4) style_ref_t
{
    tag_reference<tag_class_t::styl> style;
};

static_assert(sizeof(style_ref_t) == 8);

} // namespace blam::dimeter::ai
