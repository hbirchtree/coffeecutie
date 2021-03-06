#pragma once

#include "cblam_structures.h"

namespace blam {
namespace hsc {

namespace bc {

enum class cheats : i16
{
    cheat_all_powerups,
    cheat_all_weapons,
    cheat_spawn_warthog,
    cheat_all_vehicles,
    cheat_teleport_to_camera,
    cheat_active_camouflage,
    cheat_active_camouflage_local_player,
    cheats_load,
};

/* Xbox maps, a30 and b30 on PC */
enum class v1 : i16
{

    sentinel = -13622,
    invalid  = -1,

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

    ai_free = 156,
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
    ai_set_deaf,
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
    ai_strength,
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
    ai_disregard,

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
    game_difficulty_get = 258,
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

    cinematic_start_ = 299,
    cinematic_start  = 300,
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

    max_opcode = 550,
};

enum class v2 : i16
{
    sentinel = -13622,
    invalid  = -1,

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

    ai_free = 157,
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
    ai_set_deaf,
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
    game_difficulty_get = 258,
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

    cinematic_start_ = 299,
    cinematic_start  = 300,
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

    max_opcode = 550,
};

} // namespace bc

using xbox_opcode_t = bc::v1;
using pc_opcode_t   = bc::v2;

} // namespace hsc
} // namespace blam
