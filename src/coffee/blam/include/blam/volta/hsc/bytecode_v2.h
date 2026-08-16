#pragma once

#include <peripherals/libc/types.h>
#include <string_view>

namespace blam::hsc::bc {

/* PC (2003) maps, engine build 01.00.00.0564. Custom Edition
 * (01.00.00.0609) and the Trial release (01.00.00.0576) use the same
 * table -- verified against every opcode they share.
 *
 * Read back from the maps rather than transcribed: every group node
 * names its function in the script string segment. Entries without a
 * marker are what the maps actually use. "carried" entries were not
 * exercised by any map and keep the slot the old hand-written table
 * gave them -- unverified. _dummy_N is a slot nothing names. Entries
 * past the measured range exist only so the shared signature table
 * compiles; their values are not real opcodes.
 */
enum class v2 : libc_types::i16
{
    sentinel = -13622,
    invalid  = -1,

    begin,
    begin_random,
    if_,
    cond, /* carried */
    set_,
    and_,
    or_,
    add_,
    sub_,
    mul_,
    div_,
    min_, /* carried */
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
    sound, /* carried */
    effect, /* carried */
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
    object_set_shield, /* carried */
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
    objects_delete_by_definition, /* carried */
    sound_set_gain, /* carried */
    sound_get_gain, /* carried */
    script_recompile, /* carried */
    help, /* carried */
    _dummy_1, /* carried */
    random_range,
    real_random_range,
    numeric_countdown_timer_set,
    numeric_countdown_timer_get, /* carried */
    numeric_countdown_timer_stop,
    numeric_countdown_timer_restart, /* carried */
    breakable_surfaces_enable,
    recording_play,
    recording_play_and_delete,
    recording_play_and_hover,
    recording_kill,
    recording_time,
    object_set_ranged_attack_inhibited,
    object_set_melee_attack_inhibited,
    objects_dump_mem, /* carried */
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
    scenery_get_animation_time, /* carried */
    scenery_animation_start,
    scenery_animation_start_at_frame,
    render_effects, /* carried */
    unit_can_blink, /* carried */
    unit_open,
    unit_close,
    unit_kill,
    unit_kill_silent, /* carried */
    unit_get_custom_animation_time,
    unit_stop_custom_animation,
    unit_custom_animation_at_frame,
    custom_animation,
    custom_animation_list,
    unit_is_playing_custom_animation, /* carried */
    unit_aim_without_turning, /* carried */
    unit_set_emotion,
    unit_set_enterable_by_player,
    unit_enter_vehicle,
    vehicle_test_seat_list,
    vehicle_test_seat, /* carried */
    unit_set_emotion_animation, /* carried */
    unit_exit_vehicle,
    unit_set_maximum_vitality,
    units_set_maximum_vitality, /* carried */
    unit_set_current_vitality,
    units_set_current_vitality,
    vehicle_load_magic,
    vehicle_unload,
    magic_seat_name, /* carried */
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
    unit_set_desired_flashlight_state,
    unit_get_current_flashlight_state,
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
    _dummy_156,
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
    ai_select, /* carried */
    ai_deselect, /* carried */
    ai_spawn_actor,
    ai_set_respawn, /* carried */
    ai_set_deaf,
    ai_set_blind,
    ai_magically_see_encounter,
    ai_magically_see_players,
    ai_timer_start, /* carried */
    _dummy_176,
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
    ai_swarm_count, /* carried */
    ai_nonswarm_count,
    ai_actors,
    ai_go_to_vehicle,
    ai_go_to_vehicle_override, /* carried */
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
    ai_command_list_advance_by_unit, /* carried */
    ai_command_list_status,
    ai_is_attacking, /* carried */
    ai_force_active,
    ai_force_active_by_unit, /* carried */
    ai_set_return_state, /* carried */
    ai_set_current_state, /* carried */
    ai_playfight,
    ai_status,
    ai_reconnect,
    ai_vehicle_encounter,
    ai_vehicle_enterable_distance,
    ai_vehicle_enterable_team, /* carried */
    ai_vehicle_enterable_actor_type,
    ai_vehicle_enterable_actors, /* carried */
    ai_vehicle_enterable_disable,
    ai_look_at_object, /* carried */
    ai_stop_looking, /* carried */
    ai_automatic_migration_target,
    ai_follow_target_disable,
    ai_follow_target_players,
    ai_follow_target_unit, /* carried */
    ai_follow_target_ai,
    ai_follow_distance,
    ai_conversation,
    ai_conversation_stop,
    ai_conversation_advance,
    ai_conversation_line,
    ai_conversation_status,
    ai_link_activation,
    ai_berserk,
    ai_set_team, /* carried */
    ai_allow_charge, /* carried */
    ai_allow_dormant,
    ai_allegiance_broken,
    camera_control,
    camera_set,
    camera_set_relative,
    camera_set_animation, /* carried */
    camera_set_first_person,
    camera_set_dead,
    camera_time,
    _dummy_253,
    _dummy_254,
    game_speed,
    game_time,
    _dummy_257,
    game_difficulty_get,
    game_difficulty_get_real,
    profile_service_clear_timers, /* carried */
    map_reset, /* carried */
    map_name, /* carried */
    multiplayer_map_name, /* carried */
    game_difficulty_set, /* carried */
    crash, /* carried */
    _dummy_266,
    switch_bsp,
    structure_bsp_index,
    version, /* carried */
    playback, /* carried */
    quit,
    texture_cache_flush, /* carried */
    sound_cache_flush, /* carried */
    sound_cache_dump_to_file, /* carried */
    debug_memory, /* carried */
    debug_memory_by_file, /* carried */
    debug_memory_for_file, /* carried */
    debug_tags, /* carried */
    _dummy_279,
    _dummy_280,
    _dummy_281,
    _dummy_282,
    _dummy_283,
    _dummy_284,
    _dummy_285,
    _dummy_286,
    _dummy_287,
    ai,
    ai_dialogue_triggers,
    ai_grenades,
    ai_lines, /* carried */
    _dummy_292,
    _dummy_293,
    _dummy_294,
    _dummy_295,
    _dummy_296,
    fade_in,
    fade_out,
    cinematic_start,
    cinematic_stop,
    _dummy_301,
    cinematic_skip_start_internal,
    cinematic_skip_stop_internal,
    cinematic_show_letterbox,
    cinematic_set_title,
    cinematic_set_title_delayed, /* carried */
    cinematic_suppress_bsp_object_creation,
    _dummy_308,
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
    _dummy_322,
    _dummy_323,
    _dummy_324,
    _dummy_325,
    _dummy_326,
    _dummy_327,
    game_skip_ticks,
    sound_looping_set_alternate_, /* carried */
    sound_impulse_start,
    sound_impulse_time,
    sound_impulse_stop,
    sound_looping_predict, /* carried */
    sound_looping_start,
    sound_looping_stop,
    sound_looping_set_scale, /* carried */
    sound_looping_set_alternate,
    debug_sounds_enable, /* carried */
    debug_sounds_distances, /* carried */
    debug_sounds_wet, /* carried */
    sound_enable, /* carried */
    sound_set_master_gain, /* carried */
    sound_get_master_gain, /* carried */
    sound_set_music_gain, /* carried */
    sound_get_music_gain, /* carried */
    sound_set_effects_gain, /* carried */
    sound_get_effects_gain, /* carried */
    sound_class_set_gain,
    vehicle_hover,
    players_unzoom_all,
    player_enable_input,
    player_camera_control,
    player_action_test_reset,
    _dummy_354,
    player_action_test_primary_trigger,
    player_action_test_grenade_trigger,
    _dummy_357,
    player_action_test_action,
    player_action_test_accept,
    player_action_test_back,
    player_action_test_look_relative_up,
    player_action_test_look_relative_down,
    player_action_test_look_relative_left,
    player_action_test_look_relative_right,
    player_action_test_look_relative_all_directions,
    _dummy_366,
    player_add_equipment,
    _dummy_368,
    show_hud,
    show_hud_help_text,
    enable_hud_help_flash,
    hud_help_flash_restart, /* carried */
    activate_nav_point_flag, /* carried */
    activate_nav_point_object, /* carried */
    activate_team_nav_point_flag,
    activate_team_nav_point_object,
    deactivate_nav_point_flag, /* carried */
    deactivate_nav_point_object, /* carried */
    deactivate_team_nav_point_flag,
    deactivate_team_nav_point_object,
    hud_team_icon_set_pos, /* carried */
    hud_team_icon_set_scale, /* carried */
    hud_team_background_set_pos, /* carried */
    hud_team_background_set_scale, /* carried */
    cls,
    connect, /* carried */
    hammer_begin, /* carried */
    hammer_stop, /* carried */
    network_server_dump, /* carried */
    network_client_dump, /* carried */
    net_graph_clear, /* carried */
    net_graph_show, /* carried */
    play_update_history, /* carried */
    show_player_update_stats, /* carried */
    message_metrics_clear, /* carried */
    message_metrics_dump, /* carried */
    error_overflow_suppression, /* carried */
    structure_lens_flares_place, /* carried */
    _dummy_399,
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
    hud_clear_messages, /* carried */
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
    time_code_reset, /* carried */
    reload_shader_transparent_chicago, /* carried */
    rasterizer_reload_effects, /* carried */
    set_gamma, /* carried */
    rasterizer_fixed_function_ambient, /* carried */
    rasterizer_decals_flush, /* carried */
    rasterizer_fps_accumulate, /* carried */
    rasterizer_model_ambient_reflection_tint,
    rasterizer_lights_reset_for_new_map,
    script_screen_effect_set_value, /* carried */
    cinematic_screen_effect_start,
    cinematic_screen_effect_set_convolution,
    cinematic_screen_effect_set_filter,
    cinematic_screen_effect_set_filter_desaturation_tint,
    cinematic_screen_effect_set_video,
    cinematic_screen_effect_stop,
    cinematic_set_near_clip_distance,
    fast_setup_network_server, /* carried */
    profile_unlock_solo_levels, /* carried */
    _dummy_442,
    player0_look_invert_pitch,
    player0_look_pitch_is_inverted,
    player0_joystick_set_is_normal,
    ui_widget_show_path, /* carried */
    display_scenario_help,
    sound_enable_eax, /* carried */
    sound_eax_enabled, /* carried */
    sound_set_env, /* carried */
    sound_enable_hardware, /* carried */
    sound_set_supplementary_buffers, /* carried */
    sound_get_supplementary_buffers, /* carried */
    sound_set_rolloff, /* carried */
    sound_set_factor, /* carried */
    input_get_joy_count, /* carried */
    input_is_joy_active, /* carried */
    input_activate_joy, /* carried */
    input_deactivate_joy, /* carried */
    input_find_joystick, /* carried */
    input_show_joystick_info, /* carried */
    input_find_default, /* carried */
    config_one_control, /* carried */
    get_pitch_rate, /* carried */
    set_yaw_rate, /* carried */
    set_pitch_rate, /* carried */
    get_digital_forward_throttle, /* carried */
    set_digital_forward_throttle, /* carried */
    get_digital_strafe_throttle, /* carried */
    set_digital_strafe_throttle, /* carried */
    get_digital_yaw_increment, /* carried */
    set_digital_yaw_increment, /* carried */
    get_digital_pitch_increment, /* carried */
    set_digital_pitch_increment, /* carried */
    get_mouse_forward_threshold, /* carried */
    set_mouse_forward_threshold, /* carried */
    get_mouse_strafe_threshold, /* carried */
    set_mouse_strafe_threshold, /* carried */
    get_mouse_yaw_scale, /* carried */
    set_mouse_yaw_scale, /* carried */
    get_mouse_pitch_scale, /* carried */
    set_mouse_pitch_scale, /* carried */
    get_gamepad_forward_threshold, /* carried */
    set_gamepad_forward_threshold, /* carried */
    get_gamepad_strafe_threshold, /* carried */
    set_gamepad_strafe_threshold, /* carried */
    get_gamepad_yaw_scale, /* carried */
    set_gamepad_yaw_scale, /* carried */
    set_gamepad_pitch_scale, /* carried */
    bind, /* carried */
    unbind, /* carried */
    print_binds, /* carried */
    sv_end_game, /* carried */
    change_team, /* carried */
    sv_mapcycle, /* carried */
    sv_mapcycle_begin, /* carried */
    sv_mapcycle_add, /* carried */
    sv_mapcycle_del, /* carried */
    sv_map_next, /* carried */
    sv_map_reset, /* carried */
    sv_map, /* carried */
    rcon, /* carried */
    _dummy_503,
    _dummy_504,
    sv_rcon_password,
    sv_say,
    sv_ban, /* carried */
    sv_banlist, /* carried */
    sv_unban, /* carried */
    sv_parameters_reload, /* carried */
    sv_parameters_dump, /* carried */
    sv_status, /* carried */
    _dummy_513,
    _dummy_514,
    sv_name,
    sv_password,
    sv_log_rotation_threshold, /* carried */
    sv_log_echo_chat, /* carried */
    profile_load, /* carried */
    track_remote_player_position_updates, /* carried */
    remote_player_stats, /* carried */
    sv_get_player_action_queue_length, /* carried */
    thread_sleep, /* carried */
    checkpoint_save, /* carried */
    checkpoint_load, /* carried */
    sv_maplist, /* carried */
    sv_gamelist, /* carried */
    sv_friendly_fire, /* carried */
    sv_timelimit, /* carried */
    sv_ban_penalty, /* carried */
    sv_tk_grace, /* carried */
    sv_tk_cooldown, /* carried */
    sv_banlist_file, /* carried */
    sv_maxplayers, /* carried */
    sv_single_flag_force_reset, /* carried */
    sv_motd, /* carried */
    oid_watch, /* carried */
    oid_dump, /* carried */
    oid_status, /* carried */
    milliseconds, /* displaced by a measured entry, real opcode unknown */
    player_action_test_move_relative_all_directions, /* displaced by a measured entry, real opcode unknown */
    player_action_test_zoom, /* displaced by a measured entry, real opcode unknown */
    seconds, /* displaced by a measured entry, real opcode unknown */

    max_opcode = 550,
};

std::string_view to_string(v2 opcode);

} // namespace blam::hsc::bc
