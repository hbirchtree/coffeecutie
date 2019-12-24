#pragma once

#include "cblam_structures.h"

#if C_HAS_INCLUDE(<optional>) && __cplusplus >= 201703
#define USE_MAGIC_ENUM 1
#endif

#if USE_MAGIC_ENUM
#define MAGIC_ENUM_RANGE_MIN -10
#define MAGIC_ENUM_RANGE_MAX 540
#include "magic_enum.hpp"
#endif

namespace blam {
namespace hsc {

struct script_ref;

constexpr u16 terminator = 0xFFFF;

enum class opcode_t : i16
{
    invalid = -1,

    /* Control flow */
    begin,
    begin_random,
    if_,
    cond,
    set_,

    /* Logic */
    and_,
    or_,

    /* Arithmetic */
    add_,
    sub_,
    mul_,
    div_,

    min_,
    max_,

    equal,
    nequal,
    greater,
    less,
    gequals,
    lequals,

    sleep = 19,
    sleep_until,
    wake,
    inspect,

    unit,
    sound,
    effect,

    //    ai_debug_comms_suppress,
    //    ai_debug_comms_ignore,
    //    ai_debug_comms_focus,

    not_ = 27,

    print_,

    players,

    volume_teleport_players_not_inside,
    volume_test_object,
    volume_test_objects,
    volume_test_objects_all,

    object_teleport,
    object_set_facing,
    object_set_shield,
    object_set_permutation,
    object_create,
    object_destroy,
    object_create_anew,
    object_create_containing,
    object_create_anew_containing,
    object_destroy_containing,
    object_destroy_all,

    list_get,
    list_count = 46,

    effect_new,
    effect_new_on_object_marker,

    damage_new,
    damage_object,

    objects_can_see_object,
    objects_can_see_flag,
    objects_delete_by_definition,

    sound_set_gain,
    sound_get_gain,

    script_recompile,
    help,

    _dummy_1,

    random_range,
    real_random_range,

    numeric_countdown_timer_set,
    numeric_countdown_timer_get,
    numeric_countdown_timer_stop,
    numeric_countdown_timer_restart,

    breakable_surfaces_enable,

    recording_play,
    recording_play_and_delete,
    recording_play_and_hover,
    recording_kill,
    recording_time,

    object_set_ranged_attack_inhibited,
    object_set_melee_attack_inhibited,
    objects_dump_mem,
    object_set_collidable,
    object_set_scale,
    objects_attach,
    objects_detach,

    garbage_collect_now,

    object_cannot_take_damage,
    object_can_take_damage,

    object_beautify,

    objects_predict,
    object_type_predict,

    object_pvs_activate,
    object_pvs_set_object,
    object_pvs_set_camera,
    object_pvs_clear,

    render_lights,
    scenery_get_animation_time,
    scenery_animation_start,
    scenery_animation_start_at_frame,

    render_effects,

    unit_can_blink,
    unit_open,
    unit_close,
    unit_kill,
    unit_kill_silent,
    unit_get_custom_animation_time,
    unit_stop_custom_animation,
    unit_custom_animation_at_frame,
    custom_animation,
    custom_animation_list,
    unit_is_playing_custom_animation,
    unit_aim_without_turning,
    unit_set_emotion,
    unit_set_enterable_by_player,
    unit_enter_vehicle,
    vehicle_test_seat_list,
    vehicle_test_seat,
    unit_set_emotion_animation,
    unit_exit_vehicle,
    unit_set_maximum_vitality,
    units_set_maximum_vitality,
    unit_set_current_vitality,
    units_set_current_vitality,
    vehicle_load_magic,
    vehicle_unload,
    magic_seat_name,
    unit_set_seat,
    vehicle_riders,
    vehicle_driver,
    vehicle_gunner,
    unit_get_shield,
    unit_get_health,
    unit_get_total_grenade_count,
    unit_has_weapon,
    unit_has_weapon_readied,
    unit_doesnt_drop_items,
    unit_solo_player_integrated_night_vision_is_active,
    unit_impervious,
    unit_suspended,
    units_set_desired_flashlight_state,
    unit_set_desired_flashlight_state,
    unit_get_current_flashlight_state,

    _dummy_135,

    device_set_never_appears_locked,
    device_get_power,
    device_set_power,
    device_set_position,
    device_get_position,
    device_set_position_immediate,
    device_group_get,
    device_group_set,
    device_group_set_immediate,
    device_one_sided_set,
    device_operates_automatically_set,
    device_group_change_only_once_more_set,

    breakable_surfaces_reset,

    cheat_all_powerups,
    cheat_all_weapons,
    cheat_spawn_warthog,
    cheat_all_vehicles,
    cheat_teleport_to_camera,
    cheat_active_camouflage,
    cheat_active_camouflage_local_player,
    cheats_load,

    ai_free,
    ai_free_units,
    ai_attach,
    ai_attach_free,
    ai_detach,
    ai_place,
    ai_kill,
    ai_kill_silent,
    ai_erase,
    ai_erase_all,
    ai_select,
    ai_deselect,
    ai_spawn_actor,
    ai_set_respawn,
    ai_set_deaf = 171,
    ai_set_blind,

    ai_magically_see_encounter,
    ai_magically_see_players,

    ai_timer_start,
    ai_timer_expire,
    ai_retreat,
    ai_attack,
    ai_defend,
    ai_maneuver_enable,
    ai_maneuver,
    ai_migrate_and_speak,
    ai_migrate,
    ai_migrate_by_unit,
    ai_living_fraction,
    ai_allegiance,
    ai_allegiance_remove,
    ai_living_count,
    ai_strength = 190,
    ai_swarm_count,
    ai_nonswarm_count,
    ai_actors,
    ai_go_to_vehicle,
    ai_go_to_vehicle_override,
    ai_going_to_vehicle,
    ai_exit_vehicle,
    ai_braindead,
    ai_braindead_by_unit,
    ai_prefer_target,
    ai_disregard = 200,

    ai_renew,
    ai_teleport_to_starting_location,

    _dummy_203,
    _dummy_204,

    ai_try_to_fight_nothing = 205,
    ai_try_to_fight,
    ai_try_to_fight_player,

    ai_command_list = 208,
    ai_command_list_by_unit,
    ai_command_list_advance,
    ai_command_list_advance_by_unit,
    ai_command_list_status,
    ai_is_attacking,
    ai_force_active,
    ai_force_active_by_unit,
    ai_set_return_state,
    ai_set_current_state,
    ai_playfight,
    ai_status,

    ai_reconnect = 220,
    ai_vehicle_encounter,
    ai_vehicle_enterable_distance,
    ai_vehicle_enterable_team,
    ai_vehicle_enterable_actor_type,
    ai_vehicle_enterable_actors,
    ai_vehicle_enterable_disable,
    ai_look_at_object,
    ai_stop_looking,
    ai_automatic_migration_target,

    ai_follow_target_players = 230,
    ai_follow_target_unit,
    ai_follow_target_ai,
    ai_follow_distance,

    ai_conversation,
    ai_conversation_stop,
    ai_conversation_advance,
    ai_conversation_line,
    ai_conversation_status,
    ai_link_activation,
    ai_berserk,
    ai_set_team,
    ai_allow_charge,
    ai_allow_dormant,
    ai_allegiance_broken,

    camera_control = 246,
    camera_set,
    camera_set_relative,
    camera_set_animation,
    camera_set_first_person,
    camera_set_dead,
    camera_time,

    //    debug_camera_load,
    //    debug_camera_save,

    game_speed = 254,
    game_time,
    game_variant,
    game_difficulty_get,
    game_difficulty_get_real,

    profile_service_clear_timers,

    map_reset,
    map_name,
    multiplayer_map_name,

    game_difficulty_set,

    crash,

    switch_bsp = 267,
    structure_bsp_index,

    version,
    playback,
    quit,

    /* Unconfirmed */
    texture_cache_flush,
    sound_cache_flush,
    sound_cache_dump_to_file,

    debug_memory,
    debug_memory_by_file,
    debug_memory_for_file,
    debug_tags,

    profile_reset,
    profile_dump,
    profile_activate,
    profile_deactivate,
    profile_graph_toggle,

    debug_pvs,

    radiosity_start,
    radiosity_save,
    radiosity_debug_point,

    ai                   = 283,
    ai_dialogue_triggers = 289,
    ai_grenades,
    ai_lines,

    /* Unconfirmed */
    ai_debug_sound_point_set,
    ai_debug_vocalize,
    ai_debug_teleport_to,
    ai_debug_speak,
    ai_debug_speak_list,

    fade_in = 297,
    fade_out,

    cinematic_start = 300,
    cinematic_stop,
    cinematic_skip_start_internal,
    cinematic_skip_stop_internal,
    cinematic_abort,
    cinematic_show_letterbox,
    cinematic_set_title,
    cinematic_set_title_delayed,
    cinematic_suppress_bsp_object_creation,

    //    attract_mode_start,

    game_won,
    game_lost,
    game_safe_to_save,
    game_all_quiet,
    game_safe_to_speak,
    game_is_cooperative,
    game_save,
    game_save_cancel,
    game_save_no_timeout,
    game_save_totally_unsafe,
    game_saving,
    game_revert,
    game_reverted,

    //    core_save,
    //    core_save_name,
    //    core_load,
    //    core_load_at_startup,
    //    core_load_name,

    //    game_skip_ticks,
    sound_impulse_start_ = 323,

    sound_impulse_predict        = 329,
    sound_looping_set_alternate_ = 329,
    sound_impulse_start          = 330,
    sound_impulse_time           = 331,
    sound_impulse_stop           = 332,
    sound_looping_predict        = 333,
    sound_looping_start          = 334,
    sound_looping_stop           = 335,
    sound_looping_set_scale      = 336,
    sound_looping_set_alternate  = 337,

    debug_sounds_enable,
    debug_sounds_distances,
    debug_sounds_wet,

    sound_enable,
    sound_set_master_gain,
    sound_get_master_gain,
    sound_set_music_gain,
    sound_get_music_gain,
    sound_set_effects_gain,
    sound_get_effects_gain,
    sound_class_set_gain,

    vehicle_hover,

    players_unzoom_all,
    player_enable_input,
    player_camera_control,
    player_action_test_reset,
    player_action_test_primary_trigger,
    player_action_test_grenade_trigger,
    player_action_test_zoom,
    player_action_test_action,
    player_action_test_accept,
    player_action_test_back,
    player_action_test_look_relative_up,
    player_action_test_look_relative_down,
    player_action_test_look_relative_left,
    player_action_test_look_relative_right,
    player_action_test_look_relative_all_directions,
    player_action_test_move_relative_all_directions,
    player_add_equipment,
    debug_teleport_player,

    show_hud = 369,
    show_hud_help_text,
    enable_hud_help_flash,
    hud_help_flash_restart,

    activate_nav_point_flag,
    activate_nav_point_object,
    activate_team_nav_point_flag,
    activate_team_nav_point_object,
    deactivate_nav_point_flag,
    deactivate_nav_point_object,
    deactivate_team_nav_point_flag,
    deactivate_team_nav_point_object,
    hud_team_icon_set_pos,
    hud_team_icon_set_scale,
    hud_team_background_set_pos,
    hud_team_background_set_scale,

    cls,

    connect,
    hammer_begin,
    hammer_stop,

    network_server_dump,
    network_client_dump,
    net_graph_clear,
    net_graph_show,

    play_update_history,
    show_player_update_stats,

    message_metrics_clear,
    message_metrics_dump,

    error_overflow_suppression,

    structure_lens_flares_place,

    player_effect_set_max_translation = 400,
    player_effect_set_max_rotation,
    player_effect_set_max_rumble,
    player_effect_start,
    player_effect_stop,

    hud_show_health,
    hud_blink_health,
    hud_show_shield,
    hud_blink_shield,
    hud_show_motion_sensor,
    hud_blink_motion_sensor,
    hud_show_crosshair,
    hud_clear_messages,
    hud_set_help_text,
    hud_set_objective_text,

    hud_set_timer_time,
    hud_set_timer_warning_time,
    hud_set_timer_position,
    show_hud_timer,
    pause_hud_timer,
    hud_get_timer_ticks,
    time_code_show,
    time_code_start,
    time_code_reset,

    reload_shader_transparent_chicago,
    rasterizer_reload_effects,
    set_gamma,
    rasterizer_fixed_function_ambient,
    rasterizer_decals_flush,
    rasterizer_fps_accumulate,
    rasterizer_model_ambient_reflection_tint,
    rasterizer_lights_reset_for_new_map,

    script_screen_effect_set_value,

    cinematic_screen_effect_start,
    cinematic_screen_effect_set_convolution,
    cinematic_screen_effect_set_filter,
    cinematic_screen_effect_set_filter_desaturation_tint,
    cinematic_screen_effect_set_video,
    cinematic_screen_effect_stop,
    cinematic_set_near_clip_distance,

    fast_setup_network_server,
    profile_unlock_solo_levels,

    player0_look_invert_pitch,
    player0_look_pitch_is_inverted,
    player0_joystick_set_is_normal = 445,

    ui_widget_show_path,
    display_scenario_help,

    /* Sound commands */
    sound_enable_eax,
    sound_eax_enabled,
    sound_set_env,
    sound_enable_hardware,
    sound_set_supplementary_buffers,
    sound_get_supplementary_buffers,
    sound_set_rolloff,
    sound_set_factor,

    /* Input commands */
    input_get_joy_count,
    input_is_joy_active,
    input_activate_joy,
    input_deactivate_joy,
    input_find_joystick,
    input_show_joystick_info,
    input_find_default,
    config_one_control,
    get_pitch_rate,
    set_yaw_rate,
    set_pitch_rate,
    get_digital_forward_throttle,
    set_digital_forward_throttle,
    get_digital_strafe_throttle,
    set_digital_strafe_throttle,
    get_digital_yaw_increment,
    set_digital_yaw_increment,
    get_digital_pitch_increment,
    set_digital_pitch_increment,
    get_mouse_forward_threshold,
    set_mouse_forward_threshold,
    get_mouse_strafe_threshold,
    set_mouse_strafe_threshold,
    get_mouse_yaw_scale,
    set_mouse_yaw_scale,
    get_mouse_pitch_scale,
    set_mouse_pitch_scale,
    get_gamepad_forward_threshold,
    set_gamepad_forward_threshold,
    get_gamepad_strafe_threshold,
    set_gamepad_strafe_threshold,
    get_gamepad_yaw_scale,
    set_gamepad_yaw_scale,
    set_gamepad_pitch_scale,
    bind,
    unbind,
    print_binds,

    /* Multiplayer commands */
    sv_end_game,
    change_team,
    sv_mapcycle,
    sv_mapcycle_begin,
    sv_mapcycle_add,
    sv_mapcycle_del,
    sv_map_next,
    sv_map_reset,
    sv_map,
    rcon,
    sv_rcon_password,
    sv_say,
    sv_players,
    sv_kick,
    sv_ban,
    sv_banlist,
    sv_unban,
    sv_parameters_reload,
    sv_parameters_dump,
    sv_status,
    sv_name,
    sv_password,
    sv_log_note,
    sv_log_enabled,
    sv_log_rotation_threshold,
    sv_log_echo_chat,
    profile_load,
    track_remote_player_position_updates,
    remote_player_stats,
    sv_get_player_action_queue_length,
    thread_sleep,
    checkpoint_save,
    checkpoint_load,
    sv_maplist,
    sv_gamelist,
    sv_friendly_fire,
    sv_timelimit,
    sv_ban_penalty,
    sv_tk_grace,
    sv_tk_cooldown,
    sv_banlist_file,
    sv_maxplayers,
    sv_single_flag_force_reset,
    sv_motd,

    /* Translated object table commands (?) */
    oid_watch,
    oid_dump,
    oid_status,

    max_opcode,
};

enum class xbox_opcode_t : i16
{
};

enum class pointer_t : u16
{
};

enum class script_type_t : u16
{
    startup,
    dormant,
    continuous,
    static_,
    stub,

};

enum class node_flags_t : u16
{
    primitive = 1,
    return_val,
    variable,
    concat,
    script_param,
};

/* From BlamLib */
enum class expression_t : u16
{
    group      = 0x8,
    expression = 0x9,
    script_ref = 0xa,
    global_ref = 0xd,
    param_ref  = 0x1d,
};

/* From Blamite */
enum class function_group_t : u16
{
    macro,
    begin,
    conditional,
    cond_,
    set,
    binary,
    math,
    equal,
    nequal,
    sleep,
    sleep_forever,
    sleep_until,
    wake,
    inspect,
    cast,
    dbg_string,
    network
};

enum class type_t : i16
{
    nothing = -1,
    func_name,
    immediate_val = 2,
    passthrough   = 3,
    void_         = 4,
    bool_         = 5,
    real_         = 6,
    short_        = 7,
    long_         = 8,

    /* Not confirmed? */
    string_,
    trigger_vol = 11,
    cutscene_flag,
    cutscene_camera_pnt = 13,
    cutscene_title,
    cutscene_recording,
    device_group,
    ai,
    ai_cmd_list,
    starting_profile,
    conversation,
    navpoint,
    hud_msg,

    obj_list = 23,
    sound,
    effect,
    damage,
    loop_sound,
    anim_graph,
    actor_variant,
    dmg_effect,
    obj_def,
    game_difficulty,
    team,
    ai_def_state,
    actor_type,
    hud_corner,
    object = 37,
    unit   = 38,
    vehicle,
    weapon,
    device,
    scenery,
    object_name,
    unit_name,
    vehicle_name,
    weapon_name,
    device_name,
    scenery_name,
};

struct global : stl_types::non_copy
{
    bl_string name;
    type_t    type;
    u16       pad;
    u32       _pad;
    union
    {
        struct
        {
            u16 index;
            u16 salt;
        };
        u32 value;
    };
    u32 padding[12];
};

struct function_declaration : stl_types::non_copy
{
    bl_string     name;
    script_type_t schedule;
    type_t        type;
    u16           index;
    u16           salt;
    u8            padding[52];
};

struct script_header : stl_types::non_copy
{
    u16 unknown[7];
};

struct opcode_layout : stl_types::non_copy
{
    u16 index;
    union
    {
        opcode_t opcode;
        type_t   param_type;
    };
    type_t       ret_type;
    expression_t exp_type;
    struct
    {
        u16 ip; /*!< Points to the last parameter for the function */
        u16 salt;
    } next_op;
    u32 data_ptr;
    union
    {
        u8     data_bytes[4];
        u16    data_short[2];
        u32    data_int;
        scalar data_real;
    };

    inline bool valid() const
    {
        i16 opcode_v   = C_CAST<i16>(opcode);
        i16 opcode_min = C_CAST<i16>(opcode_t::invalid);
        i16 opcode_max = C_CAST<i16>(opcode_t::max_opcode);

        return opcode_v >= opcode_min && opcode_v < opcode_max;
    }

    inline bool branching() const
    {
        return next_op.ip != std::numeric_limits<u16>::max();
    }

    inline opcode_layout const& next(script_ref const* script) const;
};

struct script_ref;

struct opcode_iterator_end_t
{
};

constexpr opcode_iterator_end_t opcode_iterator_end;

struct opcode_iterator
    : stl_types::Iterator<stl_types::ForwardIteratorTag, opcode_layout>
{
    opcode_iterator(semantic::mem_chunk<u8 const>&& script_base);
    opcode_iterator(opcode_iterator_end_t end) :
        m_script(nullptr), m_data(), m_offset(0), m_is_end(true)
    {
    }

    inline opcode_iterator& operator++()
    {
        auto code      = m_data.at(m_offset);
        auto separator = terminator;
        auto loc =
            ::memmem(code.data, code.size, &separator, sizeof(separator));

        if(loc)
        {
            auto new_op = C_RCAST<byte_t const*>(loc);
            auto offset = new_op - code.data;
            m_offset    = offset;
        } else
        {
            m_is_end = true;
            m_offset = std::numeric_limits<u32>::max();
        }

        return *this;
    }

    inline bool operator==(opcode_iterator const& other) const
    {
        return (m_is_end && m_is_end == other.m_is_end) ||
               m_offset == other.m_offset;
    }

    inline bool operator!=(opcode_iterator const& other) const
    {
        return !(*this == other);
    }

    inline opcode_layout const& operator*() const
    {
        auto out = m_data.at(m_offset).as<opcode_layout const>();
        if(!out)
            Throw(undefined_behavior("invalid iterator"));

        return out[0];
    }

    script_ref const*             m_script;
    semantic::mem_chunk<u8 const> m_data;
    u32                           m_offset;
    bool                          m_is_end;
};

struct script_ref : stl_types::non_copy
{
    bl_string name;
    u32       sentinel_value;

    using container_type = stl_types::quick_container<opcode_iterator>;

    inline u8 const* opcode_base() const
    {
        return C_RCAST<u8 const*>(&this[1]);
    }

    inline opcode_layout const& opcode_first() const
    {
        return *C_RCAST<opcode_layout const*>(opcode_base());
    }

    inline semantic::mem_chunk<u8 const> dump(u32 count) const
    {
        return semantic::mem_chunk<u8 const>::From(opcode_base(), count);
    }
};

inline opcode_iterator::opcode_iterator(
    semantic::mem_chunk<u8 const>&& script_base) :
    m_script(C_RCAST<script_ref const*>(script_base.data)),
    m_data(script_base.at(sizeof(script_ref))), m_offset(0), m_is_end(false)
{
}

inline opcode_layout const& opcode_layout::next(script_ref const* script) const
{
    return *(
        C_RCAST<opcode_layout const*>(script->opcode_base()) + next_op.ip + 1);
}

template<typename T>
inline stl_types::CString to_string(T val)
{
#if USE_MAGIC_ENUM
    auto out = magic_enum::enum_name(val);
    if(!out.size())
        return "[invalid(" + std::to_string(C_CAST<i16>(val)) + ")]";
    return stl_types::CString(out) + "(" + std::to_string(C_CAST<i16>(val)) +
           ")";
#else
    return std::to_string(C_CAST<i16>(val));
#endif
}

struct bytecode_pointer
{
    static bytecode_pointer start_from(opcode_layout const* base, u16 ip = 0x0)
    {
        bytecode_pointer out;
        out.base       = base;
        out.current    = &base[ip];
        out.current_ip = ip;
        return out;
    }

    opcode_layout const* base;
    opcode_layout const* current;
    u16                  current_ip;

    inline u16 num_params() const
    {
        if(current->branching())
            return current->next_op.ip - current_ip - 1;
        else
            return 0;
    }
    inline opcode_layout const& param(type_t type, u16 i = 0) const
    {
        u16 first_param = current_ip + 1;
        u16 last_param  = current->next_op.ip;

        if(i > (last_param - first_param + 1))
            Throw(undefined_behavior("param out of bounds"));

        auto const& out = current[2 + i];

        if(type != out.param_type)
            Throw(undefined_behavior("param has wrong type"));

        return out;
    }
    inline void advance()
    {
        if(current->branching())
        {
            current_ip = current->next_op.ip + 1;
            current    = &base[current->next_op.ip + 1];
        } else
            current_ip = current->next_op.ip;
    }
    inline bool finished() const
    {
        return current_ip == std::numeric_limits<u16>::max();
    }
};

} // namespace hsc
} // namespace blam

namespace Coffee {
namespace Strings {

inline CString to_string(blam::hsc::opcode_t opc)
{
    return blam::hsc::to_string(opc);
}

inline CString to_string(blam::hsc::type_t type)
{
    return blam::hsc::to_string(type);
}

inline CString to_string(blam::hsc::expression_t exp)
{
    return blam::hsc::to_string(exp);
}

inline CString to_string(blam::hsc::script_type_t script_type)
{
    return blam::hsc::to_string(script_type);
}

} // namespace Strings
} // namespace Coffee
