#include "blam/volta/hsc/blam_bytecode.h"
#include "blam/volta/blam_base_types.h"
#include "blam/volta/blam_scenario.h"
#include "peripherals/semantic/chunk.h"
#include <string>
#define MAGIC_ENUM_RANGE_MIN -4
#define MAGIC_ENUM_RANGE_MAX 560
#include <magic_enum/magic_enum.hpp>

#include <blam/volta/blam_scenario.h>
#include <blam/volta/hsc/bytecode_common_v12.h>
#include <blam/volta/hsc/bytecode_v1.h>
#include <blam/volta/hsc/bytecode_v2.h>
#include <blam/volta/hsc/blam_bytecode.h>

#include <fmt/format.h>

using namespace std::string_view_literals;

namespace blam::hsc::bc {

std::string_view to_string(v1 opcode)
{
    if(opcode == v1::sentinel)
        return "[sentinel]"sv;
    return magic_enum::enum_name(opcode);
}

v1 v1_from_string(std::string_view opcode)
{
    return magic_enum::enum_cast<v1>(opcode).value_or(v1::invalid);
}

std::string_view to_string(v2 opcode)
{
    if(opcode == v2::sentinel)
        return "[sentinel]"sv;
    return magic_enum::enum_name(opcode);
}

v2 v2_from_string(std::string_view opcode)
{
    return magic_enum::enum_cast<v2>(opcode).value_or(v2::invalid);
}

std::string_view to_string(v4 opcode)
{
    if(opcode == v4::sentinel)
        return "[sentinel]"sv;
    return magic_enum::enum_name(opcode);
}

v4 v4_from_string(std::string_view opcode)
{
    return magic_enum::enum_cast<v4>(opcode).value_or(v4::invalid);
}

}

namespace blam::hsc {

std::string_view to_string(expression_t expr)
{
    return magic_enum::enum_name(expr);
}

std::string_view to_string(function_group_t group)
{
    return magic_enum::enum_name(group);
}

std::string_view to_string(script_type_t type)
{
    return magic_enum::enum_name(type);
}

std::string_view to_string(type_t type)
{
    return magic_enum::enum_name(type);
}

template<is_bytecode_variant BC>
std::string op_to_string(opcode_layout<BC> const& op)
{
    switch(op.exp_type)
    {
    case expression_t::expression:
    case expression_t::global_ref:
    case expression_t::group:
    case expression_t::param_ref:
    case expression_t::script_ref:
        break;
    default:
        return fmt::format("{:#06x}-{:#06x}: [invalid]",
            op.index,
            op.next_op.ip);
    }

    auto value = [&op] -> std::string {
        switch(op.exp_type)
        {
        case expression_t::global_ref:
            return fmt::format("global:{}", op.data_ptr);
        case expression_t::group:
            return fmt::format("opcode:{}", bc::to_string(op.opcode));
        case expression_t::param_ref:
            return fmt::format("param:{}", op.data_ptr);
        case expression_t::script_ref:
            return fmt::format("script:{}", static_cast<u16>(op.opcode));
        default:
            switch(op.param_type)
            {
            case type_t::bool_:
                return op.to_bool() ? "true" : "false";
            case type_t::real_:
                return std::to_string(op.to_real());
            case type_t::short_:
                return std::to_string(op.to_u16());
            case type_t::long_:
                return std::to_string(op.to_u32());
            default:
                return fmt::format(
                    "val:{} | ptr:{:#016x} | type:{}",
                    op.template get<i32>(),
                    op.to_ptr(),
                    to_string(op.param_type));
            }
        }
    }();

    return fmt::format(
        "{:#06x}-{:#06x}: {} # returns {}",
        op.index,
        op.next_op.ip,
        value,
        to_string(op.ret_type));
}

std::string to_string(opcode_layout<bc::v1> const& op)
{
    return op_to_string<bc::v1>(op);
}

std::string to_string(opcode_layout<bc::v2> const& op)
{
    return op_to_string<bc::v2>(op);
}

std::string to_string(opcode_layout<bc::v4> const& op)
{
    return op_to_string<bc::v4>(op);
}

std::string_view to_string(sleep_condition cond)
{
    return magic_enum::enum_name(cond);
}

std::string_view to_string(script_status stat)
{
    return magic_enum::enum_name(stat);
}

/* bytecode() starts the span 36 bytes into the script syntax data, but its
 * header is 56 bytes long: node N sits at span index N + 1. */
template<typename Dialect>
opcode_layout<Dialect> const* node_at(
    semantic::Span<const opcode_layout<Dialect>> const& bytecode, u16 index)
{
    u32 pos = static_cast<u32>(index) + 1;
    if(index == terminator || pos >= bytecode.size())
        return nullptr;
    return &bytecode[pos];
}

/* An offset only names something when it lands on a string boundary; numeric
 * literals carry a stale offset into the middle of a neighbouring string. */
std::string_view name_at(string_segment_ref const& strings, u32 offset)
{
    if(offset >= strings.data.size() ||
       (offset > 0 && strings.data[offset - 1] != '\0'))
        return {};
    return strings.data.substr(
        offset, strings.data.find('\0', offset) - offset);
}

/* A group points at its function-name node, and every node points at its next
 * sibling; walking that graph is what puts the parentheses in the right place.
 * A linear pass over the opcodes cannot know where a group ends. */
template<typename Dialect>
std::string script_to_string(
    semantic::Span<const opcode_layout<Dialect>> const& bytecode,
    string_segment_ref const&                           strings,
    u16                                                 index,
    u32                                                 depth = 0)
{
    auto const* op = node_at(bytecode, index);
    if(!op)
        return {};

    switch(op->exp_type)
    {
    case expression_t::expression:
        switch(op->param_type)
        {
        case type_t::bool_:
            return op->to_bool() ? "true" : "false";
        case type_t::real_:
            return fmt::format("{}", op->to_real());
        case type_t::short_:
            return std::to_string(op->to_u16());
        case type_t::long_:
            return std::to_string(op->to_u32());
        case type_t::string_:
            return fmt::format("\"{}\"", name_at(strings, op->to_ptr()));
        default:
            /* Object names, tag references and enum values keep their source
             * spelling in the string segment */
            if(auto name = name_at(strings, op->to_ptr()); !name.empty())
                return std::string(name);
            return std::to_string(op->template get<i32>());
        }
    case expression_t::global_ref:
    case expression_t::param_ref:
        return std::string(name_at(strings, op->to_ptr()));
    case expression_t::group:
    case expression_t::script_ref:
        break;
    default:
        return "<unknown>";
    }

    /* The value field of a group holds the datum index of its function-name
     * node, which carries the name the source used. to_*() rejects anything
     * but an expression node, so it is read directly. */
    auto const* name_op = node_at(bytecode, static_cast<u16>(op->long_));
    if(!name_op)
        return "<unknown>";
    auto name = name_at(strings, name_op->to_ptr());
    auto out  = op->exp_type == expression_t::script_ref
                   ? fmt::format("(script:{}", name)
                   : fmt::format("({}", name);
    /* Statement sequences read better one per line */
    std::string separator =
        name == "begin"sv || name == "begin_random"sv || name == "cond"sv
            ? '\n' + std::string((depth + 1) * 2, ' ')
            : " ";
    for(u16 arg = name_op->next_op.ip, guard = 0; guard < bytecode.size();
        guard++)
    {
        auto const* param = node_at(bytecode, arg);
        if(!param)
            break;
        out += separator +
               script_to_string(bytecode, strings, arg, depth + 1);
        arg = param->next_op.ip;
    }
    return out += ")";
}

template<typename Version>
std::string to_halo_script(scn::scenario<Version> const& scenario, map_ptr const& magic)
{
    auto strings = scenario.string_segment(magic);
    if(strings.has_error())
        return {};
    auto bytecode = scenario.bytecode(magic);
    std::string script;
    for(auto const& decl : scenario.function_table(magic))
    {
        script += fmt::format(
            "(script[{}] {} {}\n {}\n)\n\n",
            to_string(decl.type),
            decl.name.str(),
            to_string(decl.schedule),
            script_to_string(bytecode, strings.value(), decl.index));
    }
    return script;
}

template
std::string to_halo_script<xbox_version_t>(
    scn::scenario<xbox_version_t> const& scenario, map_ptr const& magic);

template
std::string to_halo_script<pc_version_t>(
    scn::scenario<pc_version_t> const& scenario, map_ptr const& magic);

template
std::string to_halo_script<custom_version_t>(
    scn::scenario<custom_version_t> const& scenario, map_ptr const& magic);

template
std::string to_halo_script<mcc_version_t>(
    scn::scenario<mcc_version_t> const& scenario, map_ptr const& magic);

template
std::string to_halo_script<trial_version_t>(
    scn::scenario<trial_version_t> const& scenario, map_ptr const& magic);

template<typename Bytecode>
signatures::sig_t opcode_signature(opcode_layout<Bytecode> const& op)
{
    using o = Bytecode;
    using t = type_t;

    using namespace signatures;

    switch(op.opcode)
    {
    // Requires special handling
    case o::cond:
        return sig_t(t::void_);

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
    case o::custom_animation_list:
        return sig_t(
            t::void_, t::obj_list, t::anim_graph, t::string_, t::bool_);
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
    case o::ai_kill_silent:
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
    case o::ai_teleport_to_starting_location_if_unsupported:
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
    case o::ai_migrate_by_unit:
        return sig_t(t::void_, t::object_name, t::ai);
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

    case o::object_set_shield:
        return sig_t(t::void_, t::object, t::real_);
    case o::damage_object:
        return sig_t(t::void_, t::damage, t::object);
    case o::sound_set_gain:
        return sig_t(t::void_, t::string_, t::real_);
    case o::sound_get_gain:
        return sig_t(t::real_, t::string_);

    case o::objects_delete_by_definition:
        return sig_t(t::void_, t::obj_def);

    case o::seconds:
        return sig_t(t::void_);
    }

    auto op_name = to_string(op.opcode);
    Throw(missing_signature(std::string(op_name.begin(), op_name.end())));
}

template
signatures::sig_t opcode_signature(opcode_layout<bc::v1> const& op);
template
signatures::sig_t opcode_signature(opcode_layout<bc::v2> const& op);

}

