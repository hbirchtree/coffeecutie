#pragma once

#include "bytecode_common_v12.h"

namespace blam {
namespace hsc {

constexpr bool exceptional_missing_signature = true;

constexpr u16 variable_length_params   = 0xFF;
constexpr u16 unknown_opcode_signature = 0xFF - 1;

namespace signatures {

struct sig_v
{
    const type_t return_type;
    const size_t num_params;
};

template<type_t RType = type_t::void_, type_t... Params>
struct sig_t
{
    static constexpr type_t return_type = RType;
    static constexpr auto   parameters  = std::make_tuple(Params...);
    static constexpr size_t num_parameters =
        std::tuple_size<decltype(parameters)>::value;

    static inline sig_v value = {return_type, num_parameters};

    static inline stl_types::Vector<type_t> params()
    {
        auto params_static = stl_types::MkArray(type_t::void_, Params...);

        return stl_types::Vector<type_t>(
            params_static.begin() + 1, params_static.end());
    }
};

struct param_getter
{
    using return_type = stl_types::Vector<type_t>;

    template<typename T>
    static inline return_type get()
    {
        return T::params();
    }
};

struct num_getter
{
    using return_type = signatures::sig_v;

    template<typename T>
    static inline return_type get()
    {
        return T::value;
    }
};

} // namespace signatures

template<typename Bytecode = bc::v1, typename Getter = signatures::num_getter>
inline typename Getter::return_type opcode_signature(
    opcode_layout<Bytecode> const& op)
{
    using o = Bytecode;
    using t = type_t;

    using namespace signatures;

    switch(op.opcode)
    {
    case o::cinematic_skip_stop_internal:
    case o::cinematic_skip_start_internal:
    case o::cinematic_start:
    case o::cinematic_stop:
    case o::game_revert:
    case o::game_save:
    case o::game_save_totally_unsafe:
    case o::game_save_no_timeout:
    case o::garbage_collect_now:
        return Getter::template get<sig_t<t::void_>>();

    case o::cinematic_set_title:
        return Getter::template get<sig_t<t::void_, t::cutscene_title>>();
    case o::cinematic_screen_effect_set_filter_desaturation_tint:
        return Getter::template get<
            sig_t<t::void_, t::real_, t::real_, t::real_>>();
    case o::cinematic_screen_effect_set_filter:
        return Getter::template get<sig_t<
            t::void_,
            t::real_,
            t::real_,
            t::real_,
            t::real_,
            t::bool_,
            t::real_>>();

    case o::game_all_quiet:
    case o::game_is_cooperative:
    case o::game_saving:
    case o::game_reverted:
    case o::game_won:
    case o::game_safe_to_speak:
        return Getter::template get<sig_t<t::bool_>>();

    case o::game_safe_to_save:
        return Getter::template get<sig_t<t::bool_, t::short_>>();

    case o::game_skip_ticks:
        return Getter::template get<sig_t<t::void_, t::short_>>();

    case o::game_difficulty_get:
        return Getter::template get<sig_t<t::game_difficulty>>();
    case o::game_difficulty_get_real:
        return Getter::template get<sig_t<t::game_difficulty>>();

    case o::players:
        return Getter::template get<sig_t<t::obj_list>>();

        /* Logic, arithmetic */
    case o::or_:
    case o::and_:
        return Getter::template get<sig_t<t::bool_, t::bool_, t::bool_>>();
    case o::greater:
    case o::less:
    case o::lequals:
    case o::gequals:
    case o::equal:
    case o::nequal:
        return Getter::template get<sig_t<t::bool_, t::any, t::any>>();
    case o::not_:
        return Getter::template get<sig_t<t::bool_, t::bool_>>();

    case o::min_:
    case o::max_:
        return Getter::template get<sig_t<t::any, t::any, t::any>>();

    case o::add_:
    case o::sub_:
    case o::mul_:
    case o::div_:
        return Getter::template get<sig_t<t::number, t::number, t::number>>();

    case o::real_random_range:
        return Getter::template get<sig_t<t::real_, t::real_, t::real_>>();
    case o::random_range:
        return Getter::template get<sig_t<t::short_, t::short_, t::short_>>();

        /* Flow control */
    case o::sleep:
        return Getter::template get<sig_t<t::void_, t::any>>();
    case o::sleep_until:
        return Getter::template get<sig_t<t::void_>>();
    case o::wake:
        return Getter::template get<sig_t<t::void_, t::script>>();
    case o::if_:
        return Getter::template get<sig_t<t::void_, t::bool_>>();
    case o::begin:
    case o::begin_random:
        return Getter::template get<sig_t<t::any>>();

    case o::print_:
        return Getter::template get<sig_t<t::void_, t::string_>>();

    case o::fade_in:
    case o::fade_out:
        return Getter::template get<
            sig_t<t::void_, t::real_, t::real_, t::real_, t::short_>>();

        /* List operators */
    case o::list_get:
        return Getter::template get<sig_t<t::object, t::obj_list, t::short_>>();
    case o::list_count:
        return Getter::template get<sig_t<t::short_, t::obj_list>>();

    case o::unit:
        return Getter::template get<sig_t<t::unit, t::object>>();

        /* Object operators */
    case o::object_create:
    case o::object_destroy:
    case o::object_create_anew:
        return Getter::template get<sig_t<t::void_, t::object_name>>();
    case o::object_create_containing:
    case o::object_destroy_containing:
    case o::object_create_anew_containing:
        return Getter::template get<sig_t<t::void_, t::string_>>();
    case o::object_beautify:
        return Getter::template get<sig_t<t::void_, t::object, t::bool_>>();
    case o::object_can_take_damage:
        return Getter::template get<sig_t<t::void_, t::obj_list>>();
    case o::object_destroy_all:
        return Getter::template get<sig_t<t::void_>>();
    case o::object_teleport:
    case o::object_set_facing:
        return Getter::template get<
            sig_t<t::void_, t::object_name, t::cutscene_flag>>();
    case o::objects_attach:
        return Getter::template get<sig_t<
            t::void_,
            t::object_name,
            t::string_,
            t::object_name,
            t::string_>>();
    case o::objects_detach:
        return Getter::template get<
            sig_t<t::void_, t::object_name, t::object_name>>();
    case o::object_pvs_activate:
        return Getter::template get<sig_t<t::void_, t::object>>();
    case o::object_pvs_set_camera:
        return Getter::template get<sig_t<t::void_, t::cutscene_camera_pnt>>();
    case o::object_pvs_set_object:
        return Getter::template get<sig_t<t::void_, t::object>>();
    case o::object_pvs_clear:
        return Getter::template get<sig_t<t::void_>>();

    case o::object_set_scale:
        return Getter::template get<
            sig_t<t::void_, t::object_name, t::real_, t::short_>>();
    case o::object_set_permutation:
        return Getter::template get<
            sig_t<t::void_, t::object_name, t::string_, t::string_>>();

    case o::object_cannot_take_damage:
        return Getter::template get<sig_t<t::void_, t::object_name>>();

    case o::objects_predict:
        return Getter::template get<sig_t<t::void_, t::obj_list>>();
    case o::object_type_predict:
        return Getter::template get<sig_t<t::void_, t::obj_def>>();

    case o::objects_can_see_object:
        return Getter::template get<
            sig_t<t::bool_, t::obj_list, t::object_name, t::real_>>();
    case o::objects_can_see_flag:
        return Getter::template get<
            sig_t<t::bool_, t::obj_list, t::cutscene_flag, t::real_>>();

        /* Camera controls */
    case o::camera_set:
        return Getter::template get<
            sig_t<t::void_, t::cutscene_camera_pnt, t::short_>>();
    case o::camera_set_relative:
        return Getter::template get<sig_t<
            t::void_,
            t::cutscene_camera_pnt,
            t::short_,
            t::object_name>>();
    case o::camera_time:
        return Getter::template get<sig_t<t::short_>>();
    case o::camera_set_first_person:
        return Getter::template get<sig_t<t::void_, t::unit_name>>();
    case o::camera_control:
        return Getter::template get<sig_t<t::void_, t::bool_>>();

        /* Cinematic controls */
    case o::cinematic_set_near_clip_distance:
        return Getter::template get<sig_t<t::void_, t::real_>>();
    case o::cinematic_screen_effect_start:
        return Getter::template get<sig_t<t::void_, t::bool_>>();
    case o::cinematic_screen_effect_stop:
        return Getter::template get<sig_t<>>();
    case o::cinematic_screen_effect_set_convolution:
        return Getter::template get<sig_t<
            t::void_,
            t::short_,
            t::short_,
            t::real_,
            t::real_,
            t::real_>>();
    case o::cinematic_show_letterbox:
        return Getter::template get<sig_t<t::void_, t::cutscene_title>>();
    case o::cinematic_screen_effect_set_video:
        return Getter::template get<sig_t<t::void_, t::short_, t::real_>>();

        /* Animation controls */
    case o::custom_animation:
        return Getter::template get<sig_t<
            t::void_,
            t::unit_name,
            t::anim_graph,
            t::string_,
            t::bool_>>();
    case o::unit_stop_custom_animation:
        return Getter::template get<sig_t<t::void_, t::unit_name>>();
    case o::unit_get_custom_animation_time:
        return Getter::template get<sig_t<t::real_, t::unit_name, t::real_>>();
    case o::unit_custom_animation_at_frame:
        return Getter::template get<sig_t<
            t::void_,
            t::unit_name,
            t::anim_graph,
            t::string_,
            t::bool_,
            t::short_>>();

        /* Unit recording controls */
    case o::recording_play:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::cutscene_recording>>();
    case o::recording_time:
        return Getter::template get<sig_t<t::short_, t::unit_name, t::real_>>();
    case o::recording_play_and_delete:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::cutscene_recording>>();
    case o::recording_kill:
        return Getter::template get<sig_t<t::void_, t::unit_name>>();
    case o::recording_play_and_hover:
        return Getter::template get<
            sig_t<t::void_, t::vehicle_name, t::cutscene_recording>>();

        /* Effects */
    case o::effect_new_on_object_marker:
        return Getter::template get<
            sig_t<t::void_, t::effect, t::object_name, t::string_>>();
    case o::effect_new:
        return Getter::template get<sig_t<t::effect, t::cutscene_flag>>();

    case o::damage_new:
        return Getter::template get<
            sig_t<t::void_, t::damage, t::cutscene_flag>>();

        /* Device controls */
    case o::device_set_position:
        return Getter::template get<
            sig_t<t::bool_, t::device_name, t::real_>>();
    case o::device_set_position_immediate:
        return Getter::template get<
            sig_t<t::void_, t::device_name, t::real_>>();
    case o::device_set_power:
        return Getter::template get<
            sig_t<t::void_, t::device_name, t::real_>>();
    case o::device_get_power:
    case o::device_get_position:
        return Getter::template get<sig_t<t::real_, t::device_name>>();
    case o::device_one_sided_set:
        return Getter::template get<
            sig_t<t::void_, t::device_name, t::bool_>>();
    case o::device_group_get:
        return Getter::template get<sig_t<t::real_, t::device_group>>();
    case o::device_group_set:
        return Getter::template get<
            sig_t<t::void_, t::device_group, t::real_>>();
    case o::device_operates_automatically_set:
        return Getter::template get<
            sig_t<t::void_, t::device_name, t::bool_>>();

        /* Sound controls */
    case o::sound_class_set_gain:
        return Getter::template get<
            sig_t<t::void_, t::string_, t::real_, t::short_>>();
    case o::sound_looping_start:
        return Getter::template get<
            sig_t<t::void_, t::loop_sound, t::object_name, t::real_>>();
    case o::sound_looping_stop:
        return Getter::template get<sig_t<t::void_, t::loop_sound>>();
    case o::sound_impulse_start:
        return Getter::template get<
            sig_t<t::void_, t::sound, t::object_name, t::real_>>();
    case o::sound_impulse_time:
        return Getter::template get<sig_t<t::short_, t::sound>>();
    case o::sound_impulse_stop:
        return Getter::template get<sig_t<t::void_, t::sound>>();
    case o::sound_looping_set_alternate:
    case o::sound_looping_set_alternate_:
        return Getter::template get<sig_t<t::void_, t::loop_sound, t::bool_>>();

        /* Global variables */
    case o::set_:
        return Getter::template get<sig_t<t::void_, t::nothing, t::any>>();

        /* AI operators */
    case o::ai_dialogue_triggers:
    case o::ai_grenades:
        return Getter::template get<sig_t<t::void_, t::bool_>>();
    case o::ai_attach_free:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::actor_variant>>();
    case o::ai_attach:
        return Getter::template get<sig_t<t::void_, t::unit_name, t::ai>>();
    case o::ai_detach:
    case o::ai_erase:
        return Getter::template get<sig_t<t::void_, t::unit_name>>();
    case o::ai_erase_all:
    case o::ai_reconnect:
        return Getter::template get<sig_t<>>();
    case o::ai_prefer_target:
        return Getter::template get<sig_t<t::void_, t::obj_list, t::bool_>>();
    case o::ai_conversation_advance:
    case o::ai_conversation_stop:
        return Getter::template get<sig_t<t::void_, t::conversation>>();
    case o::ai_place:
    case o::ai_kill:
    case o::ai_free:
    case o::ai_magically_see_players:
    case o::ai_attack:
    case o::ai_maneuver:
    case o::ai_defend:
    case o::ai_retreat:
    case o::ai_follow_target_unit:
    case o::ai_follow_target_players:
    case o::ai_follow_target_disable:
    case o::ai_try_to_fight_player:
    case o::ai_try_to_fight_nothing:
    case o::ai_maneuver_enable:
    case o::ai_spawn_actor:
    case o::ai_exit_vehicle:
    case o::ai_teleport_to_starting_location:
    case o::ai_magically_see_encounter:
    case o::ai_command_list_advance:
        return Getter::template get<sig_t<t::void_, t::ai>>();
    case o::ai_free_units:
        return Getter::template get<sig_t<t::void_, t::obj_list>>();
    case o::ai_strength:
        return Getter::template get<sig_t<t::real_, t::ai>>();
    case o::ai_status:
    case o::ai_living_count:
    case o::ai_nonswarm_count:
        return Getter::template get<sig_t<t::short_, t::ai>>();
    case o::ai_living_fraction:
        return Getter::template get<sig_t<t::real_, t::ai>>();
    case o::ai_actors:
        return Getter::template get<sig_t<t::obj_list, t::ai>>();
    case o::ai_set_blind:
    case o::ai_playfight:
    case o::ai_set_deaf:
    case o::ai_set_respawn:
    case o::ai_berserk:
    case o::ai_braindead:
    case o::ai_force_active:
    case o::ai_migrate_and_speak:
        return Getter::template get<sig_t<t::void_, t::ai, t::bool_>>();
    case o::ai_migrate:
    case o::ai_try_to_fight:
    case o::ai_follow_target_ai:
    case o::ai_link_activation:
        return Getter::template get<sig_t<t::void_, t::ai, t::ai>>();
    case o::ai_vehicle_encounter:
        return Getter::template get<sig_t<t::void_, t::unit, t::ai>>();
    case o::ai_vehicle_enterable_distance:
        return Getter::template get<sig_t<t::void_, t::unit_name, t::real_>>();
    case o::ai_vehicle_enterable_actor_type:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::actor_type>>();
    case o::ai_vehicle_enterable_disable:
        return Getter::template get<sig_t<t::void_, t::unit_name>>();
    case o::ai_go_to_vehicle:
        return Getter::template get<
            sig_t<t::void_, t::ai, t::unit, t::string_>>();
    case o::ai_command_list:
        return Getter::template get<sig_t<t::void_, t::ai, t::ai_cmd_list>>();
    case o::ai_command_list_by_unit:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::ai_cmd_list>>();
    case o::ai_command_list_status:
        return Getter::template get<sig_t<t::short_, t::obj_list>>();
    case o::ai_allegiance:
    case o::ai_allegiance_remove:
        return Getter::template get<sig_t<t::void_, t::team, t::team>>();
    case o::ai_conversation_status:
        return Getter::template get<sig_t<t::short_, t::conversation>>();
    case o::ai_conversation:
        return Getter::template get<sig_t<t::void_, t::conversation>>();

    /* Effects */
    case o::player_effect_set_max_rumble:
        return Getter::template get<sig_t<t::void_, t::real_, t::real_>>();
    case o::player_effect_set_max_translation:
    case o::player_effect_set_max_rotation:
        return Getter::template get<
            sig_t<t::void_, t::real_, t::real_, t::real_>>();
    case o::player_effect_start:
        return Getter::template get<sig_t<t::void_, t::real_, t::real_>>();
    case o::player_effect_stop:
        return Getter::template get<sig_t<t::void_, t::real_>>();

        /* Unit controls */
    case o::unit_enter_vehicle:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::vehicle_name, t::short_>>();
    case o::vehicle_hover:
        return Getter::template get<
            sig_t<t::void_, t::vehicle_name, t::bool_>>();
    case o::vehicle_test_seat_list:
        return Getter::template get<
            sig_t<t::bool_, t::vehicle_name, t::string_, t::obj_list>>();
    case o::vehicle_load_magic:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::string_, t::obj_list>>();
    case o::vehicle_unload:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::string_>>();
    case o::vehicle_riders:
        return Getter::template get<sig_t<t::obj_list, t::unit_name>>();

    case o::unit_set_seat:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::string_>>();
    case o::unit_suspended:
        return Getter::template get<sig_t<t::void_, t::unit_name, t::bool_>>();
    case o::unit_set_enterable_by_player:
        return Getter::template get<sig_t<t::void_, t::unit_name, t::bool_>>();
    case o::unit_exit_vehicle:
        return Getter::template get<sig_t<t::void_, t::unit_name>>();
    case o::unit_impervious:
        return Getter::template get<sig_t<t::void_, t::obj_list, t::bool_>>();
    case o::unit_set_maximum_vitality:
    case o::unit_set_current_vitality:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::real_, t::real_>>();
    case o::units_set_desired_flashlight_state:
        return Getter::template get<sig_t<t::bool_, t::short_>>();
    case o::units_set_current_vitality:
        return Getter::template get<
            sig_t<t::void_, t::obj_list, t::real_, t::real_>>();
    case o::unit_get_health:
        return Getter::template get<sig_t<t::real_, t::unit_name>>();
    case o::unit_close:
    case o::unit_open:
        return Getter::template get<sig_t<t::void_, t::unit_name>>();
    case o::unit_set_emotion:
        return Getter::template get<sig_t<t::void_, t::unit_name, t::short_>>();
    case o::unit_solo_player_integrated_night_vision_is_active:
        return Getter::template get<sig_t<t::void_>>();

        /* Trigger volume controls */
    case o::volume_test_object:
        return Getter::template get<
            sig_t<t::bool_, t::trigger_vol, t::object_name>>();
    case o::volume_test_objects:
    case o::volume_test_objects_all:
        return Getter::template get<
            sig_t<t::bool_, t::trigger_vol, t::obj_list>>();

    case o::switch_bsp:
        return Getter::template get<sig_t<t::void_, t::short_>>();
    case o::structure_bsp_index:
        return Getter::template get<sig_t<t::short_>>();

        /* Player controls */
    case o::show_hud:
    case o::show_hud_help_text:
    case o::hud_show_crosshair:
    case o::hud_show_health:
    case o::hud_show_shield:
    case o::hud_show_motion_sensor:
    case o::hud_blink_health:
    case o::hud_blink_shield:
    case o::hud_blink_motion_sensor:
    case o::enable_hud_help_flash:
        return Getter::template get<sig_t<t::void_, t::bool_>>();
    case o::hud_set_help_text:
        return Getter::template get<sig_t<t::void_, t::hud_msg>>();
    case o::hud_set_objective_text:
        return Getter::template get<sig_t<t::void_, t::hud_msg>>();
    case o::display_scenario_help:
        return Getter::template get<sig_t<t::void_, t::short_>>();

    case o::player_enable_input:
        return Getter::template get<sig_t<t::void_, t::bool_>>();
    case o::player_action_test_reset:
        return Getter::template get<sig_t<t::void_>>();
    case o::player_action_test_back:
    case o::player_action_test_look_relative_left:
    case o::player_action_test_look_relative_right:
    case o::player_action_test_look_relative_down:
    case o::player_action_test_look_relative_up:
        return Getter::template get<sig_t<t::bool_>>();
    case o::player_action_test_move_relative_all_directions:
    case o::player_action_test_look_relative_all_directions:
    case o::player_action_test_accept:
    case o::player_action_test_zoom:
    case o::player_action_test_grenade_trigger:
        return Getter::template get<sig_t<t::bool_, t::short_>>();
    case o::player_camera_control:
        return Getter::template get<sig_t<t::void_, t::bool_>>();

    case o::player0_joystick_set_is_normal:
    case o::player0_look_pitch_is_inverted:
        return Getter::template get<sig_t<t::bool_>>();
    case o::player0_look_invert_pitch:
        return Getter::template get<sig_t<t::void_, t::bool_>>();

    case o::cls:
        return Getter::template get<sig_t<t::void_>>();

        /* Debugging */
    case o::inspect:
        return Getter::template get<sig_t<t::void_, t::any>>();

    case o::rasterizer_lights_reset_for_new_map:
        return Getter::template get<sig_t<t::void_>>();

    case o::deactivate_team_nav_point_flag:
        return Getter::template get<
            sig_t<t::void_, t::team, t::cutscene_flag>>();
    case o::activate_team_nav_point_flag:
        return Getter::template get<sig_t<
            t::void_,
            t::navpoint,
            t::team,
            t::cutscene_flag,
            t::real_>>();

    default:
        break;
    }

    auto op_name = magic_enum::enum_name(op.opcode);
    Throw(missing_signature(std::string(op_name.begin(), op_name.end())));
}

template<typename BC>
inline u16 param_count(opcode_layout<BC> const& op)
{
    if(op.exp_type == expression_t::expression)
        return 0;

    switch(op.opcode)
    {
    case BC::sleep:
    case BC::sleep_until:
        return variable_length_params;
    default:
        break;
    }

    auto out = opcode_signature(op);

    if(out.return_type == type_t::unevaluated)
        return unknown_opcode_signature;

    return out.num_params;
}

} // namespace hsc
} // namespace blam
