#pragma once

#include <peripherals/libc/types.h>
#include <string_view>

namespace blam::hsc::bc {

/* Xbox (2001) maps, engine build 01.10.12.2276.
 *
 * PC and Xbox agree exactly up to opcode 148 and drift from ai_free
 * (PC 157 / Xbox 156) onwards, reaching an offset of 33 by opcode 430.
 */
enum class v1 : libc_types::i16
{
    sentinel = -13622,
    invalid  = -1,

    begin,
    begin_random,
    if_,
    cond, /* inferred */
    set_,
    and_,
    or_,
    add_,
    sub_,
    mul_,
    div_,
    min_, /* inferred */
    max_,
    equal,
    nequal,
    greater,
    less,
    gequals,
    lequals,
    sleep,
    sleep_until,
    wake,
    inspect,
    unit,
    sound, /* inferred */
    effect, /* inferred */
    _dummy_26,
    not_,
    print_,
    players,
    volume_teleport_players_not_inside,
    volume_test_object,
    volume_test_objects,
    volume_test_objects_all,
    object_teleport,
    object_set_facing,
    object_set_shield, /* inferred */
    object_set_permutation,
    object_create,
    object_destroy,
    object_create_anew,
    object_create_containing,
    object_create_anew_containing,
    object_destroy_containing,
    object_destroy_all,
    list_get,
    list_count,
    effect_new,
    effect_new_on_object_marker,
    damage_new,
    damage_object,
    objects_can_see_object,
    objects_can_see_flag,
    objects_delete_by_definition, /* inferred */
    sound_set_gain, /* inferred */
    sound_get_gain, /* inferred */
    script_recompile, /* inferred */
    help, /* inferred */
    _dummy_1, /* inferred */
    random_range,
    real_random_range,
    numeric_countdown_timer_set,
    numeric_countdown_timer_get, /* inferred */
    numeric_countdown_timer_stop,
    numeric_countdown_timer_restart, /* inferred */
    breakable_surfaces_enable,
    recording_play,
    recording_play_and_delete,
    recording_play_and_hover,
    recording_kill,
    recording_time,
    object_set_ranged_attack_inhibited,
    object_set_melee_attack_inhibited,
    objects_dump_mem, /* inferred */
    object_set_collideable,
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
    scenery_get_animation_time, /* inferred */
    scenery_animation_start,
    scenery_animation_start_at_frame,
    render_effects, /* inferred */
    unit_can_blink, /* inferred */
    unit_open,
    unit_close,
    unit_kill, /* inferred */
    unit_kill_silent, /* inferred */
    unit_get_custom_animation_time,
    unit_stop_custom_animation,
    unit_custom_animation_at_frame,
    custom_animation,
    custom_animation_list,
    unit_is_playing_custom_animation, /* inferred */
    unit_aim_without_turning, /* inferred */
    unit_set_emotion,
    unit_set_enterable_by_player,
    unit_enter_vehicle,
    vehicle_test_seat_list,
    vehicle_test_seat, /* inferred */
    unit_set_emotion_animation, /* inferred */
    unit_exit_vehicle,
    unit_set_maximum_vitality,
    units_set_maximum_vitality, /* inferred */
    unit_set_current_vitality,
    units_set_current_vitality,
    vehicle_load_magic,
    vehicle_unload,
    magic_seat_name, /* inferred */
    unit_set_seat,
    _dummy_120,
    vehicle_riders,
    vehicle_driver,
    vehicle_gunner,
    unit_get_health,
    unit_get_shield,
    unit_get_total_grenade_count,
    unit_has_weapon,
    unit_has_weapon_readied,
    unit_doesnt_drop_items,
    unit_impervious,
    unit_suspended,
    unit_solo_player_integrated_night_vision_is_active,
    units_set_desired_flashlight_state,
    unit_set_desired_flashlight_state, /* inferred */
    unit_get_current_flashlight_state, /* inferred */
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
    _dummy_149,
    _dummy_150,
    _dummy_151,
    _dummy_152,
    _dummy_153,
    _dummy_154,
    _dummy_155,
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
    ai_select, /* inferred */
    ai_deselect, /* inferred */
    ai_spawn_actor,
    ai_set_respawn, /* inferred */
    ai_set_deaf,
    ai_set_blind,
    ai_magically_see_encounter,
    ai_magically_see_players,
    ai_timer_start, /* inferred */
    _dummy_175,
    ai_timer_expire,
    ai_attack,
    ai_defend,
    ai_retreat,
    ai_maneuver,
    ai_maneuver_enable,
    ai_migrate,
    ai_migrate_and_speak,
    ai_migrate_by_unit,
    ai_allegiance,
    ai_allegiance_remove,
    ai_living_count,
    ai_living_fraction,
    ai_strength,
    ai_swarm_count, /* inferred */
    ai_nonswarm_count,
    ai_actors,
    ai_go_to_vehicle,
    ai_go_to_vehicle_override, /* inferred */
    ai_going_to_vehicle,
    ai_exit_vehicle,
    ai_braindead,
    ai_braindead_by_unit,
    ai_disregard,
    ai_prefer_target,
    ai_teleport_to_starting_location,
    ai_teleport_to_starting_location_if_unsupported,
    ai_renew,
    ai_try_to_fight_nothing,
    ai_try_to_fight,
    ai_try_to_fight_player,
    ai_command_list,
    ai_command_list_by_unit,
    ai_command_list_advance,
    ai_command_list_advance_by_unit, /* inferred */
    ai_command_list_status,
    ai_is_attacking, /* inferred */
    ai_force_active,
    ai_force_active_by_unit, /* inferred */
    ai_set_return_state, /* inferred */
    ai_set_current_state, /* inferred */
    ai_playfight,
    ai_status,
    ai_reconnect,
    ai_vehicle_encounter,
    ai_vehicle_enterable_distance,
    ai_vehicle_enterable_team, /* inferred */
    ai_vehicle_enterable_actor_type,
    ai_vehicle_enterable_actors, /* inferred */
    ai_vehicle_enterable_disable,
    ai_look_at_object, /* inferred */
    ai_stop_looking, /* inferred */
    ai_automatic_migration_target,
    ai_follow_target_disable,
    ai_follow_target_players,
    ai_follow_target_unit, /* inferred */
    ai_follow_target_ai,
    ai_follow_distance,
    ai_conversation,
    ai_conversation_stop,
    ai_conversation_advance,
    ai_conversation_line,
    ai_conversation_status,
    ai_link_activation,
    ai_berserk,
    ai_set_team, /* inferred */
    ai_allow_charge, /* inferred */
    ai_allow_dormant,
    ai_allegiance_broken,
    camera_control,
    camera_set,
    camera_set_relative,
    camera_set_animation, /* inferred */
    camera_set_first_person,
    camera_set_dead,
    camera_time,
    _dummy_252,
    _dummy_253,
    game_speed,
    game_time,
    _dummy_256,
    game_difficulty_get,
    game_difficulty_get_real,
    profile_service_clear_timers, /* inferred */
    map_reset, /* inferred */
    map_name, /* inferred */
    multiplayer_map_name, /* inferred */
    game_difficulty_set, /* inferred */
    switch_bsp,
    structure_bsp_index,
    version, /* inferred */
    playback, /* inferred */
    quit, /* inferred */
    texture_cache_flush, /* inferred */
    sound_cache_flush, /* inferred */
    sound_cache_dump_to_file, /* inferred */
    debug_memory, /* inferred */
    debug_memory_by_file, /* inferred */
    debug_memory_for_file, /* inferred */
    debug_tags, /* inferred */
    _dummy_276,
    _dummy_277,
    _dummy_278,
    _dummy_279,
    _dummy_280,
    _dummy_281,
    _dummy_282,
    ai,
    ai_dialogue_triggers,
    ai_grenades,
    ai_lines, /* inferred */
    _dummy_287,
    _dummy_288,
    _dummy_289,
    _dummy_290,
    _dummy_291,
    fade_in,
    fade_out,
    cinematic_start,
    cinematic_stop,
    cinematic_skip_start_internal,
    cinematic_skip_stop_internal,
    cinematic_show_letterbox,
    cinematic_set_title,
    cinematic_set_title_delayed, /* inferred */
    cinematic_suppress_bsp_object_creation,
    _dummy_302,
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
    _dummy_316,
    _dummy_317,
    _dummy_318,
    _dummy_319,
    _dummy_320,
    _dummy_321,
    game_skip_ticks,
    sound_impulse_start,
    sound_impulse_time,
    sound_impulse_stop,
    sound_looping_predict, /* inferred */
    sound_looping_start,
    sound_looping_stop,
    sound_looping_set_scale, /* inferred */
    sound_looping_set_alternate,
    debug_sounds_enable, /* inferred */
    debug_sounds_distances, /* inferred */
    debug_sounds_wet, /* inferred */
    sound_enable, /* inferred */
    sound_class_set_gain,
    vehicle_hover,
    players_unzoom_all,
    player_enable_input,
    player_camera_control,
    player_action_test_reset,
    _dummy_341,
    player_action_test_primary_trigger,
    player_action_test_grenade_trigger,
    _dummy_344,
    player_action_test_action,
    player_action_test_accept,
    player_action_test_back,
    player_action_test_look_relative_up,
    player_action_test_look_relative_down,
    player_action_test_look_relative_left,
    player_action_test_look_relative_right,
    player_action_test_look_relative_all_directions,
    _dummy_353,
    player_add_equipment,
    _dummy_355,
    show_hud,
    show_hud_help_text,
    enable_hud_help_flash,
    hud_help_flash_restart, /* inferred */
    activate_nav_point_flag, /* inferred */
    activate_nav_point_object, /* inferred */
    activate_team_nav_point_flag,
    activate_team_nav_point_object,
    deactivate_nav_point_flag, /* inferred */
    deactivate_nav_point_object, /* inferred */
    deactivate_team_nav_point_flag,
    deactivate_team_nav_point_object,
    cls,
    hud_team_icon_set_scale, /* inferred */
    hud_team_background_set_pos, /* inferred */
    player_effect_set_max_translation,
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
    hud_clear_messages, /* inferred */
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
    time_code_reset, /* inferred */
    reload_shader_transparent_chicago, /* inferred */
    rasterizer_reload_effects, /* inferred */
    rasterizer_model_ambient_reflection_tint,
    rasterizer_lights_reset_for_new_map,
    rasterizer_decals_flush, /* inferred */
    cinematic_screen_effect_start,
    cinematic_screen_effect_set_convolution,
    cinematic_screen_effect_set_filter,
    cinematic_screen_effect_set_filter_desaturation_tint,
    cinematic_screen_effect_set_video,
    cinematic_screen_effect_stop,
    cinematic_set_near_clip_distance,
    fast_setup_network_server, /* inferred */
    profile_unlock_solo_levels, /* inferred */
    _dummy_409,
    _dummy_410,
    player0_look_invert_pitch,
    player0_look_pitch_is_inverted,
    player0_joystick_set_is_normal,
    ui_widget_show_path, /* inferred */
    display_scenario_help,
    sound_enable_eax, /* inferred */
    sound_eax_enabled, /* inferred */
    sound_set_env, /* inferred */
    sound_enable_hardware, /* inferred */
    sound_set_supplementary_buffers, /* inferred */
    sound_get_supplementary_buffers, /* inferred */
    sound_set_rolloff, /* inferred */
    sound_set_factor, /* inferred */
    input_get_joy_count, /* inferred */
    input_is_joy_active, /* inferred */
    input_activate_joy, /* inferred */
    input_deactivate_joy, /* inferred */
    input_find_joystick, /* inferred */
    input_show_joystick_info, /* inferred */
    input_find_default, /* inferred */
    config_one_control, /* inferred */
    get_pitch_rate, /* inferred */
    set_yaw_rate, /* inferred */
    set_pitch_rate, /* inferred */
    get_digital_forward_throttle, /* inferred */
    set_digital_forward_throttle, /* inferred */
    get_digital_strafe_throttle, /* inferred */
    set_digital_strafe_throttle, /* inferred */
    get_digital_yaw_increment, /* inferred */
    set_digital_yaw_increment, /* inferred */
    get_digital_pitch_increment, /* inferred */
    set_digital_pitch_increment, /* inferred */
    get_mouse_forward_threshold, /* inferred */
    set_mouse_forward_threshold, /* inferred */
    get_mouse_strafe_threshold, /* inferred */
    set_mouse_strafe_threshold, /* inferred */
    get_mouse_yaw_scale, /* inferred */
    set_mouse_yaw_scale, /* inferred */
    get_mouse_pitch_scale, /* inferred */
    set_mouse_pitch_scale, /* inferred */
    get_gamepad_forward_threshold, /* inferred */
    set_gamepad_forward_threshold, /* inferred */
    get_gamepad_strafe_threshold, /* inferred */
    set_gamepad_strafe_threshold, /* inferred */
    get_gamepad_yaw_scale, /* inferred */
    set_gamepad_yaw_scale, /* inferred */
    set_gamepad_pitch_scale, /* inferred */
    bind, /* inferred */
    unbind, /* inferred */
    print_binds, /* inferred */
    sv_end_game, /* inferred */
    change_team, /* inferred */
    sv_mapcycle, /* inferred */
    sv_mapcycle_begin, /* inferred */
    sv_mapcycle_add, /* inferred */
    sv_mapcycle_del, /* inferred */
    sv_map_next, /* inferred */
    sv_map_reset, /* inferred */
    sv_map, /* inferred */
    rcon, /* inferred */
    sv_rcon_password, /* inferred */
    sv_say, /* inferred */
    sv_players, /* inferred */
    sv_kick, /* inferred */
    sv_ban, /* inferred */
    sv_banlist, /* inferred */
    sv_unban, /* inferred */
    sv_parameters_reload, /* inferred */
    sv_parameters_dump, /* inferred */
    sv_status, /* inferred */
    sv_name, /* inferred */
    sv_password, /* inferred */
    sv_log_note, /* inferred */
    sv_log_enabled, /* inferred */
    sv_log_rotation_threshold, /* inferred */
    sv_log_echo_chat, /* inferred */
    profile_load, /* inferred */
    track_remote_player_position_updates, /* inferred */
    remote_player_stats, /* inferred */
    sv_get_player_action_queue_length, /* inferred */
    thread_sleep, /* inferred */
    checkpoint_save, /* inferred */
    checkpoint_load, /* inferred */
    sv_maplist, /* inferred */
    sv_gamelist, /* inferred */
    sv_friendly_fire, /* inferred */
    sv_timelimit, /* inferred */
    sv_ban_penalty, /* inferred */
    sv_tk_grace, /* inferred */
    sv_tk_cooldown, /* inferred */
    sv_banlist_file, /* inferred */
    sv_maxplayers, /* inferred */
    sv_single_flag_force_reset, /* inferred */
    sv_motd, /* inferred */
    oid_watch, /* inferred */
    oid_dump, /* inferred */
    oid_status, /* inferred */

    max_opcode = 550,

    /* Named by the signature table, but no map emits them and their real
     * opcode is unknown; past max_opcode so they cannot claim a real slot. */
    milliseconds,
    player_action_test_move_relative_all_directions,
    player_action_test_zoom,
    seconds,
    sound_looping_set_alternate_,
};

std::string_view to_string(v1 opcode);

} // namespace blam::hsc::bc
