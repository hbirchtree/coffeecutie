#pragma once

#include "bytecode_common_v12.h"
#include "peripherals/stl/enumerate.h"
#include "peripherals/stl/types.h"

#include <array>
#include <span>

namespace blam {
namespace hsc {

constexpr bool exceptional_missing_signature = true;

constexpr u16 variable_length_params   = 0xFF;
constexpr u16 unknown_opcode_signature = 0xFF - 1;

namespace signatures {

struct sig_t
{
    template<typename... T>
    sig_t(type_t return_type, T... param_type)
        : return_type(return_type)
        , num_params(sizeof...(T))
    {
        // Avoid dynamic allocation here, but it means we need to do some
        // trickery
        static_assert(
            sizeof...(T) <= 10, "signature has more params than anticipated");
        auto params =
            stl_types::make_array<type_t, T...>(std::move(param_type)...);
        for(size_t i = 0; i < sizeof...(T); i++)
            param_types[i] = params[i];
    }

    const type_t           return_type;
    const size_t           num_params;
    std::array<type_t, 10> param_types;

    inline auto params() const
    {
        return std::span<const type_t>(param_types.begin(), num_params);
    }

    inline auto enumerate_params() const
    {
        return stl_types::enumerate(params());
    }
};

} // namespace signatures

template<typename Bytecode = bc::v1>
inline signatures::sig_t opcode_signature(opcode_layout<Bytecode> const& op)
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
        return sig_t(t::void_);

    case o::cinematic_set_title:
        return sig_t(t::void_, t::cutscene_title);
    case o::cinematic_screen_effect_set_filter_desaturation_tint:
        return sig_t(t::void_, t::real_, t::real_, t::real_);
    case o::cinematic_screen_effect_set_filter:
        return sig_t(
            t::void_,
            t::real_,
            t::real_,
            t::real_,
            t::real_,
            t::bool_,
            t::real_);

    case o::game_all_quiet:
    case o::game_is_cooperative:
    case o::game_saving:
    case o::game_reverted:
    case o::game_won:
    case o::game_safe_to_speak:
        return sig_t(t::bool_);

    case o::game_safe_to_save:
        return sig_t(t::bool_, t::short_);

        // case o::game_skip_ticks:
        //     return sig_t(t::void_, t::short_);

    case o::game_difficulty_get:
        return sig_t(t::game_difficulty);
    case o::game_difficulty_get_real:
        return sig_t(t::game_difficulty);

    case o::players:
        return sig_t(t::obj_list);

        /* Logic, arithmetic */
    case o::or_:
    case o::and_:
        return sig_t(t::bool_, t::bool_, t::bool_);
    case o::greater:
    case o::less:
    case o::lequals:
    case o::gequals:
    case o::equal:
    case o::nequal:
        return sig_t(t::bool_, t::any, t::any);
    case o::not_:
        return sig_t(t::bool_, t::bool_);

    case o::min_:
    case o::max_:
        return sig_t(t::any, t::any, t::any);

    case o::add_:
    case o::sub_:
    case o::mul_:
    case o::div_:
        return sig_t(t::number, t::number, t::number);

    case o::real_random_range:
        return sig_t(t::real_, t::real_, t::real_);
    case o::random_range:
        return sig_t(t::short_, t::short_, t::short_);

        /* Flow control */
    case o::thread_sleep:
        return sig_t(t::void_, t::long_);
    case o::sleep:
        // TODO: Find out how to express multiple prototypes for opcode
        return sig_t(t::void_, t::any);
    case o::sleep_until:
        return sig_t(t::void_);
    case o::wake:
        return sig_t(t::void_, t::script);
    case o::if_:
        return sig_t(t::void_, t::bool_);
    case o::begin:
    case o::begin_random:
        return sig_t(t::any);

    case o::print_:
        return sig_t(t::void_, t::string_);

    case o::fade_in:
    case o::fade_out:
        return sig_t(t::void_, t::real_, t::real_, t::real_, t::short_);

        /* List operators */
    case o::list_get:
        return sig_t(t::object, t::obj_list, t::short_);
    case o::list_count:
        return sig_t(t::short_, t::obj_list);

    case o::unit:
        return sig_t(t::unit, t::object);

        /* Object operators */
    case o::object_create:
    case o::object_destroy:
    case o::object_create_anew:
        return sig_t(t::void_, t::object_name);
    case o::object_create_containing:
    case o::object_destroy_containing:
    case o::object_create_anew_containing:
        return sig_t(t::void_, t::string_);
    case o::object_beautify:
        return sig_t(t::void_, t::object, t::bool_);
    case o::object_can_take_damage:
        return sig_t(t::void_, t::obj_list);
    case o::object_destroy_all:
        return sig_t(t::void_);
    case o::object_teleport:
    case o::object_set_facing:
        return sig_t(t::void_, t::object_name, t::cutscene_flag);
    case o::objects_attach:
        return sig_t(
            t::void_, t::object_name, t::string_, t::object_name, t::string_);
    case o::objects_detach:
        return sig_t(t::void_, t::object_name, t::object_name);
    case o::object_pvs_activate:
        return sig_t(t::void_, t::object);
    case o::object_pvs_set_camera:
        return sig_t(t::void_, t::cutscene_camera_pnt);
    case o::object_pvs_set_object:
        return sig_t(t::void_, t::object);
    case o::object_pvs_clear:
        return sig_t(t::void_);

    case o::object_set_scale:
        return sig_t(t::void_, t::object_name, t::real_, t::short_);
    case o::object_set_permutation:
        return sig_t(t::void_, t::object_name, t::string_, t::string_);

    case o::object_cannot_take_damage:
        return sig_t(t::void_, t::object_name);

    case o::objects_predict:
        return sig_t(t::void_, t::obj_list);
    case o::object_type_predict:
        return sig_t(t::void_, t::obj_def);

    case o::objects_can_see_object:
        return sig_t(t::bool_, t::obj_list, t::object_name, t::real_);
    case o::objects_can_see_flag:
        return sig_t(t::bool_, t::obj_list, t::cutscene_flag, t::real_);

        /* Camera controls */
    case o::camera_set:
        return sig_t(t::void_, t::cutscene_camera_pnt, t::short_);
    case o::camera_set_relative:
        return sig_t(
            t::void_, t::cutscene_camera_pnt, t::short_, t::object_name);
    case o::camera_time:
        return sig_t(t::short_);
    case o::camera_set_first_person:
        return sig_t(t::void_, t::unit_name);
    case o::camera_control:
        return sig_t(t::void_, t::bool_, t::cutscene_camera_pnt);

        /* Cinematic controls */
    case o::cinematic_set_near_clip_distance:
        return sig_t(t::void_, t::real_);
    case o::cinematic_screen_effect_start:
        return sig_t(t::void_, t::bool_);
    case o::cinematic_screen_effect_stop:
        return sig_t(t::void_);
    case o::cinematic_screen_effect_set_convolution:
        return sig_t(
            t::void_, t::short_, t::short_, t::real_, t::real_, t::real_);
    case o::cinematic_show_letterbox:
        return sig_t(t::void_, t::cutscene_title);
    case o::cinematic_screen_effect_set_video:
        return sig_t(t::void_, t::short_, t::real_);

        /* Animation controls */
    case o::custom_animation:
        return sig_t(
            t::void_, t::unit_name, t::anim_graph, t::string_, t::bool_);
    case o::unit_stop_custom_animation:
        return sig_t(t::void_, t::unit_name);
    case o::unit_get_custom_animation_time:
        return sig_t(t::real_, t::unit_name, t::real_);
    case o::unit_custom_animation_at_frame:
        return sig_t(
            t::void_,
            t::unit_name,
            t::anim_graph,
            t::string_,
            t::bool_,
            t::short_);

        /* Unit recording controls */
    case o::recording_play:
        return sig_t(t::void_, t::unit_name, t::cutscene_recording);
    case o::recording_time:
        return sig_t(t::short_, t::unit_name, t::real_);
    case o::recording_play_and_delete:
        return sig_t(t::void_, t::unit_name, t::cutscene_recording);
    case o::recording_kill:
        return sig_t(t::void_, t::unit_name);
    case o::recording_play_and_hover:
        return sig_t(t::void_, t::vehicle_name, t::cutscene_recording);

        /* Effects */
    case o::effect_new_on_object_marker:
        return sig_t(t::void_, t::effect, t::object_name, t::string_);
    case o::effect_new:
        return sig_t(t::effect, t::cutscene_flag);

    case o::damage_new:
        return sig_t(t::void_, t::damage, t::cutscene_flag);

        /* Device controls */
    case o::device_set_position:
        return sig_t(t::bool_, t::device_name, t::real_);
    case o::device_set_position_immediate:
        return sig_t(t::void_, t::device_name, t::real_);
    case o::device_set_power:
        return sig_t(t::void_, t::device_name, t::real_);
    case o::device_get_power:
    case o::device_get_position:
        return sig_t(t::real_, t::device_name);
    case o::device_one_sided_set:
        return sig_t(t::void_, t::device_name, t::bool_);
    case o::device_group_get:
        return sig_t(t::real_, t::device_group);
    case o::device_group_set:
        return sig_t(t::void_, t::device_group, t::real_);
    case o::device_operates_automatically_set:
        return sig_t(t::void_, t::device_name, t::bool_);

        /* Sound controls */
    case o::sound_class_set_gain:
        return sig_t(t::void_, t::string_, t::real_, t::short_);
    case o::sound_looping_start:
        return sig_t(t::void_, t::loop_sound, t::object_name, t::real_);
    case o::sound_looping_stop:
        return sig_t(t::void_, t::loop_sound);
    case o::sound_impulse_start:
        return sig_t(t::void_, t::sound, t::object_name, t::real_);
    case o::sound_impulse_time:
        return sig_t(t::short_, t::sound);
    case o::sound_impulse_stop:
        return sig_t(t::void_, t::sound);
    case o::sound_looping_set_alternate:
    case o::sound_looping_set_alternate_:
        return sig_t(t::void_, t::loop_sound, t::bool_);

        /* Global variables */
    case o::set_:
        return sig_t(t::void_, t::nothing, t::any);

        /* AI operators */
    case o::ai_dialogue_triggers:
    case o::ai_grenades:
        return sig_t(t::void_, t::bool_);
    case o::ai_attach_free:
        return sig_t(t::void_, t::unit_name, t::actor_variant);
    case o::ai_attach:
        return sig_t(t::void_, t::unit_name, t::ai);
    case o::ai_detach:
    case o::ai_erase:
        return sig_t(t::void_, t::unit_name);
    case o::ai_erase_all:
    case o::ai_reconnect:
        return sig_t(t::void_);
    case o::ai_prefer_target:
        return sig_t(t::void_, t::obj_list, t::bool_);
    case o::ai_conversation_advance:
    case o::ai_conversation_stop:
        return sig_t(t::void_, t::conversation);
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
    // case o::ai_follow_target_disable:
    case o::ai_try_to_fight_player:
    case o::ai_try_to_fight_nothing:
    case o::ai_maneuver_enable:
    case o::ai_spawn_actor:
    case o::ai_exit_vehicle:
    case o::ai_teleport_to_starting_location:
    case o::ai_magically_see_encounter:
    case o::ai_command_list_advance:
        return sig_t(t::void_, t::ai);
    case o::ai_free_units:
        return sig_t(t::void_, t::obj_list);
    case o::ai_strength:
        return sig_t(t::real_, t::ai);
    case o::ai_status:
    case o::ai_living_count:
    case o::ai_nonswarm_count:
        return sig_t(t::short_, t::ai);
    case o::ai_living_fraction:
        return sig_t(t::real_, t::ai);
    case o::ai_actors:
        return sig_t(t::obj_list, t::ai);
    case o::ai_set_blind:
    case o::ai_playfight:
    case o::ai_set_deaf:
    case o::ai_set_respawn:
    case o::ai_berserk:
    case o::ai_braindead:
    case o::ai_force_active:
    case o::ai_migrate_and_speak:
        return sig_t(t::void_, t::ai, t::bool_);
    case o::ai_migrate:
    case o::ai_try_to_fight:
    case o::ai_follow_target_ai:
    case o::ai_link_activation:
        return sig_t(t::void_, t::ai, t::ai);
    case o::ai_vehicle_encounter:
        return sig_t(t::void_, t::unit, t::ai);
    case o::ai_vehicle_enterable_distance:
        return sig_t(t::void_, t::unit_name, t::real_);
    case o::ai_vehicle_enterable_actor_type:
        return sig_t(t::void_, t::unit_name, t::actor_type);
    case o::ai_vehicle_enterable_disable:
        return sig_t(t::void_, t::unit_name);
    case o::ai_go_to_vehicle:
        return sig_t(t::void_, t::ai, t::unit, t::string_);
    case o::ai_command_list:
        return sig_t(t::void_, t::ai, t::ai_cmd_list);
    case o::ai_command_list_by_unit:
        return sig_t(t::void_, t::unit_name, t::ai_cmd_list);
    case o::ai_command_list_status:
        return sig_t(t::short_, t::obj_list);
    case o::ai_allegiance:
    case o::ai_allegiance_remove:
        return sig_t(t::void_, t::team, t::team);
    case o::ai_conversation_status:
        return sig_t(t::short_, t::conversation);
    case o::ai_conversation:
        return sig_t(t::void_, t::conversation);

    /* Effects */
    case o::player_effect_set_max_rumble:
        return sig_t(t::void_, t::real_, t::real_);
    case o::player_effect_set_max_translation:
    case o::player_effect_set_max_rotation:
        return sig_t(t::void_, t::real_, t::real_, t::real_);
    case o::player_effect_start:
        return sig_t(t::void_, t::real_, t::real_);
    case o::player_effect_stop:
        return sig_t(t::void_, t::real_);

        /* Unit controls */
    case o::unit_enter_vehicle:
        return sig_t(t::void_, t::unit_name, t::vehicle_name, t::short_);
    case o::vehicle_hover:
        return sig_t(t::void_, t::vehicle_name, t::bool_);
    case o::vehicle_test_seat_list:
        return sig_t(t::bool_, t::vehicle_name, t::string_, t::obj_list);
    case o::vehicle_load_magic:
        return sig_t(t::void_, t::unit_name, t::string_, t::obj_list);
    case o::vehicle_unload:
        return sig_t(t::void_, t::unit_name, t::string_);
    case o::vehicle_riders:
        return sig_t(t::obj_list, t::unit_name);

    case o::unit_set_seat:
        return sig_t(t::void_, t::unit_name, t::string_);
    case o::unit_suspended:
        return sig_t(t::void_, t::unit_name, t::bool_);
    case o::unit_set_enterable_by_player:
        return sig_t(t::void_, t::unit_name, t::bool_);
    case o::unit_exit_vehicle:
        return sig_t(t::void_, t::unit_name);
    case o::unit_impervious:
        return sig_t(t::void_, t::obj_list, t::bool_);
    case o::unit_set_maximum_vitality:
    case o::unit_set_current_vitality:
        return sig_t(t::void_, t::unit_name, t::real_, t::real_);
    case o::units_set_desired_flashlight_state:
        return sig_t(t::bool_, t::short_);
    case o::units_set_current_vitality:
        return sig_t(t::void_, t::obj_list, t::real_, t::real_);
    case o::unit_get_health:
        return sig_t(t::real_, t::unit_name);
    case o::unit_close:
    case o::unit_open:
        return sig_t(t::void_, t::unit_name);
    case o::unit_set_emotion:
        return sig_t(t::void_, t::unit_name, t::short_);
    case o::unit_solo_player_integrated_night_vision_is_active:
        return sig_t(t::void_);

        /* Trigger volume controls */
    case o::volume_test_object:
        return sig_t(t::bool_, t::trigger_vol, t::object_name);
    case o::volume_test_objects:
    case o::volume_test_objects_all:
        return sig_t(t::bool_, t::trigger_vol, t::obj_list);
    case o::volume_teleport_players_not_inside:
        return sig_t(t::void_, t::trigger_vol, t::cutscene_flag);

    case o::switch_bsp:
        return sig_t(t::void_, t::short_);
    case o::structure_bsp_index:
        return sig_t(t::short_);

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
        return sig_t(t::void_, t::bool_);
    case o::hud_set_help_text:
        return sig_t(t::void_, t::hud_msg);
    case o::hud_set_objective_text:
        return sig_t(t::void_, t::hud_msg);
    case o::display_scenario_help:
        return sig_t(t::void_, t::short_);

    case o::player_enable_input:
        return sig_t(t::void_, t::bool_);
    case o::player_action_test_reset:
        return sig_t(t::void_);
    case o::player_action_test_back:
    case o::player_action_test_look_relative_left:
    // case o::player_action_test_look_relative_right:
    case o::player_action_test_look_relative_down:
    case o::player_action_test_look_relative_up:
        return sig_t(t::bool_);
    // case o::player_action_test_move_relative_all_directions:
    case o::player_action_test_look_relative_all_directions:
    case o::player_action_test_accept:
    case o::player_action_test_zoom:
    case o::player_action_test_grenade_trigger:
        return sig_t(t::bool_, t::short_);
        // case o::player_camera_control:
        //     return sig_t(t::void_, t::bool_);

    case o::player0_joystick_set_is_normal:
    case o::player0_look_pitch_is_inverted:
        return sig_t(t::bool_);
    case o::player0_look_invert_pitch:
        return sig_t(t::void_, t::bool_);

    case o::cls:
        return sig_t(t::void_);

        /* Debugging */
    case o::inspect:
        return sig_t(t::void_, t::any);

    case o::rasterizer_lights_reset_for_new_map:
        return sig_t(t::void_);

    case o::deactivate_team_nav_point_flag:
        return sig_t(t::void_, t::team, t::cutscene_flag);
    case o::activate_team_nav_point_flag:
        return sig_t(
            t::void_, t::navpoint, t::team, t::cutscene_flag, t::real_);

    default:
        break;
    }

    auto op_name = to_string(op.opcode);
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
