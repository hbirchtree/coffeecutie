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

#include "cblam_bytecode_opcodes.h"

namespace blam {
namespace hsc {

template<typename BC>
struct script_ref;

constexpr u16 terminator = 0xFFFF;

enum class script_type_t : u16
{
    startup,    /*!< Executed at beginning */
    dormant,    /*!< Script that sleeps until (wake) is called on it */
    continuous, /*!< Script that runs at every tick */
    static_,    /*!< Script that may be called like a function */
    stub,       /*!< Unknown? */

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
    unevaluated = -4,
    number      = -3,
    any         = -2,
    nothing     = -1,
    func_name,
    immediate_val = 2,
    passthrough   = 3,
    void_         = 4,
    bool_         = 5,
    real_         = 6,
    short_        = 7,
    long_         = 8,
    string_       = 9,

    script = 10, /* Used by (wake [script]) */

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

    inline bool is_callable() const
    {
        return schedule == script_type_t::static_;
    }

    inline bool is_scheduled() const
    {
        switch(schedule)
        {
        case script_type_t::startup:
        case script_type_t::dormant:
        case script_type_t::continuous:
            return true;
        default:
            return false;
        }
    }
};

struct script_header : stl_types::non_copy
{
    u16 unknown[7];
};

template<typename Bytecode>
struct opcode_layout
{
    u16 index;
    union
    {
        Bytecode opcode;
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
        scalar        data_real;
        i32           data_int;
        Array<i16, 2> data_short;
        Array<u8, 4>  data_bytes;
    };

    inline bool valid() const
    {
        i16 opcode_v   = C_CAST<i16>(opcode);
        i16 opcode_min = C_CAST<i16>(Bytecode::invalid);
        i16 opcode_max = C_CAST<i16>(Bytecode::max_opcode);

        return opcode_v >= opcode_min && opcode_v < opcode_max;
    }

    inline bool branching() const
    {
        return next_op.ip != std::numeric_limits<u16>::max();
    }

    inline void verify_expression() const
    {
        if(exp_type != expression_t::expression)
            Throw(undefined_behavior("not an expression"));
    }

    inline u8 to_bool() const
    {
        verify_expression();

        if(param_type != type_t::bool_)
            Throw(undefined_behavior("invalid bool"));

        return data_bytes[0];
    }
    inline i16 to_u16() const
    {
        verify_expression();

        if(param_type != type_t::short_)
            Throw(undefined_behavior("invalid u16"));

        return data_short[0];
    }
    inline i32 to_u32() const
    {
        verify_expression();

        if(param_type != type_t::long_)
            Throw(undefined_behavior("invalid u32"));

        return data_int;
    }
    inline scalar to_real() const
    {
        verify_expression();

        if(param_type != type_t::real_)
            Throw(undefined_behavior("invalid real"));

        return data_real;
    }
    inline cstring to_str(string_segment_ref const& string_seg) const
    {
        verify_expression();

        if(param_type != type_t::string_)
            Throw(undefined_behavior("invalid string"));

        return (*string_seg.at(data_ptr)).str();
    }

    /* Value types for return */
    static inline opcode_layout void_()
    {
        opcode_layout out;
        out.ret_type = out.param_type = type_t::void_;
        out.exp_type                  = expression_t::expression;
        out.opcode                    = Bytecode::invalid;
        out.data_int                  = 0;
        out.next_op.ip                = terminator;
        out.next_op.salt              = terminator;
        out.data_ptr                  = 0;
        return out;
    }
    static inline opcode_layout typed_(type_t rtype)
    {
        opcode_layout out = void_();
        out.ret_type = out.param_type = rtype;
        return out;
    }
};

constexpr u16 variable_length_params = 0xFF;

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
    case o::cinematic_start_:
    case o::cinematic_stop:
    case o::cinematic_abort:
    case o::game_revert:
    case o::game_save:
    case o::game_save_totally_unsafe:
    case o::game_save_no_timeout:
    case o::garbage_collect_now:
        return Getter::template get<sig_t<>>();

    case o::game_is_cooperative:
    case o::game_saving:
    case o::game_reverted:
    case o::game_won:
        return Getter::template get<sig_t<t::bool_>>();

    case o::game_difficulty_get:
        return Getter::template get<sig_t<t::game_difficulty>>();
    case o::game_difficulty_get_real:
        return Getter::template get<sig_t<t::game_difficulty>>();

    case o::players:
        return Getter::template get<sig_t<t::obj_list>>();

        /* Logic, arithmetic */
    case o::or_:
    case o::and_:
    case o::greater:
    case o::less:
    case o::lequals:
    case o::gequals:
    case o::equal:
    case o::nequal:
        return Getter::template get<sig_t<t::bool_, t::bool_, t::bool_>>();
    case o::not_:
        return Getter::template get<sig_t<t::bool_, t::bool_>>();

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
        return Getter::template get<
            sig_t<t::void_, t::bool_, t::unevaluated>>();
    case o::begin:
        return Getter::template get<sig_t<>>();

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
        return Getter::template get<sig_t<t::void_, t::object_name>>();

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

        /* Device controls */
    case o::device_set_position:
    case o::device_set_position_immediate:
        return Getter::template get<
            sig_t<t::void_, t::device_name, t::real_>>();
    case o::device_set_power:
        return Getter::template get<
            sig_t<t::void_, t::device_name, t::real_>>();
    case o::device_get_power:
    case o::device_get_position:
        return Getter::template get<sig_t<t::real_, t::device_name>>();

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
    case o::ai_erase:
        return Getter::template get<sig_t<t::void_, t::unit_name>>();
    case o::ai_erase_all:
    case o::ai_reconnect:
        return Getter::template get<sig_t<>>();
    case o::ai_prefer_target:
        return Getter::template get<sig_t<t::void_, t::obj_list, t::bool_>>();
    case o::ai_conversation_stop:
        return Getter::template get<sig_t<t::void_, t::conversation>>();
    case o::ai_place:
    case o::ai_actors:
    case o::ai_magically_see_players:
    case o::ai_maneuver:
    case o::ai_follow_target_unit:
    case o::ai_try_to_fight_player:
    case o::ai_attack:
    case o::ai_spawn_actor:
        return Getter::template get<sig_t<t::void_, t::ai>>();

    case o::ai_set_blind:
    case o::ai_set_deaf:
    case o::ai_set_respawn:
        return Getter::template get<sig_t<t::void_, t::ai, t::bool_>>();

    case o::ai_living_count:
    case o::ai_magically_see_encounter:
        return Getter::template get<sig_t<t::short_, t::ai>>();

    case o::ai_command_list_by_unit:
        return Getter::template get<
            sig_t<t::void_, t::unit_name, t::ai_cmd_list>>();

    case o::ai_migrate:
    case o::ai_try_to_fight:
        return Getter::template get<sig_t<t::void_, t::ai, t::ai>>();

    case o::ai_allegiance:
    case o::ai_allegiance_remove:
        return Getter::template get<sig_t<t::void_, t::team, t::team>>();

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
    case o::units_set_current_vitality:
        return Getter::template get<
            sig_t<t::void_, t::obj_list, t::real_, t::real_>>();

        /* Trigger volume controls */
    case o::volume_test_object:
        return Getter::template get<
            sig_t<t::bool_, t::trigger_vol, t::object_name>>();
    case o::volume_test_objects:
        return Getter::template get<
            sig_t<t::bool_, t::trigger_vol, t::obj_list>>();

    case o::switch_bsp:
        return Getter::template get<sig_t<t::void_, t::short_>>();
    case o::structure_bsp_index:
        return Getter::template get<sig_t<t::short_>>();

        /* Player controls */
    case o::show_hud:
    case o::show_hud_help_text:
        return Getter::template get<sig_t<t::void_, t::bool_>>();
    case o::hud_set_help_text:
        return Getter::template get<sig_t<t::void_, t::hud_msg>>();
    case o::hud_set_objective_text:
        return Getter::template get<sig_t<t::void_, t::hud_msg>>();

    case o::player_enable_input:
        return Getter::template get<sig_t<t::void_, t::bool_>>();

        /* Debugging */
    case o::inspect:
        return Getter::template get<sig_t<>>();

    default:
        break;
    }
    Throw(undefined_behavior("signature not implemented"));
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

    return opcode_signature(op).num_params;
}

inline bool is_number(type_t t)
{
    return t == type_t::short_ || t == type_t::long_ || t == type_t::real_;
}

inline bool is_any(type_t t)
{
    return t == type_t::any;
}

inline bool match_type(type_t t1, type_t t2)
{
    using t = type_t;

    if(is_any(t1) || is_any(t2))
        return true;

    if(t1 == t::number || t2 == t::number)
        return is_number(t1) || is_number(t2);

    return t1 == t2;
}

struct opcode_iterator_end_t
{
};

constexpr opcode_iterator_end_t opcode_iterator_end;

template<typename BC>
struct opcode_iterator
    : stl_types::Iterator<stl_types::ForwardIteratorTag, opcode_layout<BC>>
{
    opcode_iterator(semantic::mem_chunk<u8 const>&& script_base);
    opcode_iterator(opcode_iterator_end_t end) :
        m_script(nullptr), m_data(), m_offset(0), m_is_end(true)
    {
    }

    inline opcode_iterator& operator++()
    {
        auto code      = *m_data.at(m_offset);
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

    inline opcode_layout<BC> const& operator*() const
    {
        auto out = (*m_data.at(m_offset)).template as<opcode_layout<BC> const>();
        if(!out)
            Throw(undefined_behavior("invalid iterator"));

        return out[0];
    }

    script_ref<BC> const*         m_script;
    semantic::mem_chunk<u8 const> m_data;
    u32                           m_offset;
    bool                          m_is_end;
};

template<typename BC>
struct script_ref : stl_types::non_copy
{
    bl_string name;
    u32       sentinel_value;

    using container_type = stl_types::quick_container<opcode_iterator<BC>>;

    inline u8 const* opcode_base() const
    {
        return C_RCAST<u8 const*>(&this[1]);
    }

    inline opcode_layout<BC> const& opcode_first() const
    {
        return *C_RCAST<opcode_layout<BC> const*>(opcode_base());
    }

    inline semantic::mem_chunk<u8 const> dump(u32 count) const
    {
        return semantic::mem_chunk<u8 const>::From(opcode_base(), count);
    }
};

template<typename BC>
inline opcode_iterator<BC>::opcode_iterator(
    semantic::mem_chunk<u8 const>&& script_base) :
    m_script(C_RCAST<script_ref<BC> const*>(script_base.data)),
    m_data(*script_base.at(sizeof(script_ref<BC>))), m_offset(0),
    m_is_end(false)
{
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

template<typename BC>
struct bytecode_pointer;

struct script_environment
{
    string_segment_ref const*                       strings;
    semantic::mem_chunk<function_declaration const> scripts;
    semantic::mem_chunk<global const>               globals;
};

enum class sleep_condition
{
    undefined,
    timer,
    expression,
};

enum class script_status
{
    running,  /*!< Continuous or startup script */
    ready,    /*!< Signaled for running */
    sleeping, /*!< Waiting on timer or event */
    dormant,  /*!< Scripts put to sleep forever */
    finished, /*!< Startup scripts end up here */
};

struct global_value
{
    global_value(global const* desc) : desc(desc)
    {
        long_ = 0;
    }

    global const* desc;
    union
    {
        u32  long_;
        u16  short_;
        bool bool_;
    };
};

template<typename Bytecode>
struct wait_condition
{
    sleep_condition condition;
    u16             expression;
    u32             time; /*!< Time in milliseconds */
    u16             tickrate;
};

template<typename Bytecode>
struct script_context
{
    struct object
    {
    };

    using wait_condition = hsc::wait_condition<Bytecode>;

    struct script_state
    {
        script_state(function_declaration const* func) : function(func)
        {
            switch(func->schedule)
            {
            case script_type_t::startup:
            case script_type_t::continuous:
                status = script_status::running;
                break;
            default:
                status = script_status::dormant;
                break;
            }

            ip = func->index;
        }

        function_declaration const* function;
        script_status               status;
        u16                         ip;
        u16                         script_start, script_end;
        stl_types::Deque<u16>       link_register;
        wait_condition              condition;

        inline bool is_ready() const
        {
            return status == script_status::ready ||
                   status == script_status::running;
        }
        inline bool is_inactive() const
        {
            return status == script_status::sleeping ||
                   status == script_status::dormant ||
                   status == script_status::finished;
        }
        inline stl_types::CString name() const
        {
            return function->name.str();
        }
    };

    struct procedure_data
    {
        function_declaration const* function;
        u16                         script_start, script_end;
    };

    stl_types::Map<u32, procedure_data>        procedures;
    stl_types::Map<stl_types::CString, object> objects;
    stl_types::Map<u32, global_value>          globals;
    stl_types::Map<u32, script_state>          scripts;

    inline function_declaration const* function_by_ptr(u32 ptr) const
    {
        auto it = procedures.find(ptr);

        if(it == procedures.end())
            return nullptr;

        return (*it).second;
    }

    inline script_state* script_by_ptr(u32 ptr)
    {
        auto it = scripts.find(ptr);

        if(it == scripts.end())
            return nullptr;

        return &(*it).second;
    }

    inline global_value* global_by_ptr(u32 ptr)
    {
        auto it = globals.find(ptr);

        if(it == globals.end())
            return nullptr;

        return &(*it).second;
    }

    inline object* object_by_name(u32 ptr)
    {
        auto it = objects.find(ptr);

        if(it == objects.end())
            return nullptr;

        return &(*it).second;
    }

    inline object& create_object(u32 ptr)
    {
        return objects.insert({ptr, object{}});
    }

    inline void set_script_state(
        u32 ptr, script_status state, wait_condition cond)
    {
        auto script = script_by_ptr(ptr);

        if(!script)
            Throw(undefined_behavior("failed to change script state"));

        script->status = state;
    }
};

enum class script_eval_result
{
    running,
    sleeping
};

template<typename BC>
struct bytecode_pointer
{
    using ptr_type        = u16;
    using bytecode_t      = BC;
    using opcode_layout_t = opcode_layout<BC>;
    using context_t       = script_context<bytecode_t>;
    using eval            = script_eval_result;
    using script_state_t  = typename context_t::script_state;
    using bytecode_ptr    = bytecode_pointer<BC>;

    struct result_t
    {
        opcode_layout<BC> result;
        eval              state;

        /* For sleep conditions */
        wait_condition<BC> condition;

        static result_t return_(opcode_layout_t const& out = {})
        {
            return {out, eval::running};
        }
        static result_t sleep_timeout(u32 time)
        {
            return {{},
                    eval::sleeping,
                    {sleep_condition::timer, terminator, time, 0}};
        }
        static result_t sleep_condition(u16 expr, u16 tick = 1)
        {
            return {{},
                    eval::sleeping,
                    {sleep_condition::expression, expr, 0, tick}};
        }
    };

    using opcode_handler_t =
        stl_types::Function<result_t(bytecode_ptr&, opcode_layout_t const&)>;

    static bytecode_ptr start_from(
        script_environment const& env,
        opcode_layout_t const*    base,
        ptr_type                  ip = 0x0)
    {
        bytecode_pointer out;
        out.base       = base;
        out.current    = &base[ip];
        out.current_ip = ip;

        /* Initialize script context */
        for(auto const& script : env.scripts)
        {
            auto idx = env.strings->get_index(script.name);

            if(idx == 0)
                idx = (script.salt << 16) + script.index;

            u16 script_start = base[script.index + 2].next_op.ip + 1,
                script_end   = script.index;

            if(script.is_callable())
                out.context.procedures.emplace(
                    idx,
                    typename context_t::procedure_data{
                        &script, script_start, script_end});
            else if(script.is_scheduled())
            {
                out.context.scripts.emplace(
                    idx, typename context_t::script_state(&script));

                auto it = out.context.scripts.find(idx);

                if(it == out.context.scripts.end())
                    continue;

                /* Rewind the instruction pointer to start of script */
                typename context_t::script_state& state = (*it).second;

                state.ip           = script_start;
                state.script_start = script_start;
                state.script_end   = script_end;
            }
        }

        for(auto const& global : env.globals)
        {
            auto idx = env.strings->get_index(global.name);
            out.context.globals.emplace(idx, global_value(&global));
        }

        return out;
    }

    context_t              context;
    opcode_layout_t const* base;
    opcode_layout_t const* current;
    ptr_type               current_ip;
    ptr_type               script_start, script_end;

    stl_types::Deque<ptr_type> link_register;
    /*!< Return addresses, for calling procedures */

    stl_types::Vector<opcode_layout_t> value_stack;
    /*!< Values for consumption by functions */

    inline u16 num_params() const
    {
        if(current->branching())
            return current->next_op.ip - current_ip - 1;
        else
            return 0;
    }
    inline opcode_layout_t const& param(type_t type, ptr_type i = 0) const
    {
        ptr_type first_param = current_ip + 1;
        ptr_type last_param  = current->next_op.ip;

        if(i > (last_param - first_param + 1))
            Throw(undefined_behavior("param out of bounds"));

        auto out = *value_stack.at(value_stack.size() - i - 1);

        if(!match_type(type, out.ret_type))
            Throw(undefined_behavior("param has wrong type"));

        return out;
    }
    inline result_t evaluate(
        opcode_layout_t const& op, opcode_handler_t const& handler)
    {
        opcode_layout_t out = opcode_layout_t::void_();
        switch(op.exp_type)
        {
        case expression_t::expression:
        {
            switch(op.param_type)
            {
            case type_t::real_:
                out.data_real = op.data_real;
                break;
            case type_t::long_:
                out.data_int = op.data_int;
                break;
            case type_t::short_:
                out.data_short = op.data_short;
                break;
            case type_t::bool_:
                out.data_bytes = op.data_bytes;
                break;
            default:
                out.data_ptr = op.data_ptr;
                break;
            }
            out.ret_type = out.param_type = op.param_type;

            break;
        }
        case expression_t::group:
        {
            auto start_params = value_stack.size();
            auto param_state  = evaluate_params(handler, op);
            auto op_params    = value_stack.size() - start_params;

            if(param_state.state == eval::sleeping)
                return param_state;

            switch(op.opcode)
            {
            case BC::begin:
            {
                break;
            }
            case BC::set_:
            {
                auto global_ =
                    context.global_by_ptr(param(type_t::any).data_ptr);
                break;
            }
            case BC::wake:
            {
                context.set_script_state(
                    param(type_t::any).data_ptr, script_status::running, {});

                break;
            }
            case BC::sleep_until:
            {
                opcode_layout_t const* condition = nullptr;
                u16                    tick_rate = 1;

                switch(op_params)
                {
                case 1:
                    condition = &param(type_t::any);
                    break;
                case 2:
                    condition = &param(type_t::any, 1);
                    tick_rate = param(type_t::short_, 0).to_u16();
                    break;
                case 3:
                    condition = &param(type_t::any, 2);
                    tick_rate = param(type_t::short_, 1).to_u16();
                    break;
                }

                return result_t::sleep_condition(condition - base, tick_rate);
            }
            case BC::sleep:
            {
                opcode_layout_t const* timeout = nullptr;
                opcode_layout_t const* script  = nullptr;

                switch(op_params)
                {
                case 1:
                {
                    auto const& first = param(type_t::any);
                    switch(first.param_type)
                    {
                    case type_t::real_:
                    case type_t::short_:
                    {
                        timeout = &first;
                        break;
                    }
                    case type_t::script:
                    {
                        script = &first;
                        break;
                    }
                    }
                    break;
                }
                case 2:
                    timeout = &param(type_t::number, 1);
                    script  = &param(type_t::script, 0);
                    break;
                default:
                    Throw(undefined_behavior("unhandled sleep case"));
                }

                i16 time = -1;
                if(timeout)
                    switch(timeout->param_type)
                    {
                    case type_t::real_:
                        time = deref_real(*timeout);
                        break;
                    case type_t::short_:
                        time = deref_i16(*timeout);
                        break;
                    }

                if(script)
                    context.set_script_state(
                        script->data_ptr,
                        script_status::sleeping,
                        {sleep_condition::timer,
                         terminator,
                         C_FCAST<u32>(time),
                         0});
                else
                    return result_t::sleep_timeout(time);

                break;
            }
            case BC::if_:
            {
                auto cond = param(type_t::bool_, 1);
                auto expr = param(type_t::unevaluated, 0);

                auto result = false;

                switch(cond.exp_type)
                {
                case expression_t::expression:
                    result = cond.to_bool();
                    break;
                case expression_t::global_ref:
                    result = context.global_by_ptr(cond.data_ptr)->bool_;
                    break;
                }

                if(result)
                    evaluate(base[expr.next_op.ip], handler);

                break;
            }
            case BC::equal:
            case BC::nequal:
            {
                auto left  = param(type_t::any, 1);
                auto right = param(type_t::any, 0);

                out = opcode_layout_t::typed_(type_t::bool_);

                break;
            }
            case BC::and_:
            case BC::or_:
            {
                auto left  = param(type_t::bool_, 1);
                auto right = param(type_t::bool_, 0);

                out = opcode_layout_t::typed_(type_t::bool_);

                break;
            }
            default:
            {
                auto grp = handler(*this, op);

                if(grp.state == eval::sleeping)
                    return grp;

                out = grp.result;

                break;
            }
            }

            if(param_count(op) == variable_length_params)
                pop_params(op_params);
            else
                pop_params(param_count(op));

            break;
        }
        case expression_t::global_ref:
        case expression_t::script_ref:
        {
            out = op;
            break;
        }
        default:
            Throw(undefined_behavior("unhandled expression type"));
            break;
        }
        return result_t::return_(out);
    }
    inline result_t evaluate_params(
        opcode_handler_t const& handler, opcode_layout_t const& op)
    {
        auto num = param_count(op);

        if(num == 0)
            return result_t::return_({});

        auto param_types = opcode_signature<BC, signatures::param_getter>(op);
        u16  param_i     = 0;

        if(num == variable_length_params)
        {
            /* Functions like sleep_until() may take an arbitrary amount of
             * parameters, but points to the last parameter */

            auto end_addr = current->next_op.ip;

            if(end_addr == terminator)
                end_addr = script_end;

            jump(current_ip + 2);

            do
            {
                auto val = evaluate(*current, handler);

                if(val.state == eval::sleeping)
                {
                    return_();
                    return val;
                }

                value_stack.push_back(val.result);
                advance();
                param_i++;
            } while(current_ip <= end_addr);

        } else
        {
            auto start_param = value_stack.size();
            jump(current_ip + 2);

            do
            {
                switch(param_types[param_i])
                {
                case type_t::unevaluated:
                {
                    value_stack.push_back(
                        opcode_layout_t::typed_(type_t::unevaluated));
                    value_stack.back().next_op.ip = current_ip;
                    break;
                }
                default:
                {
                    auto val = evaluate(*current, handler);

                    if(val.state == eval::sleeping)
                    {
                        return_();
                        return val;
                    }

                    value_stack.push_back(val.result);
                    break;
                }
                }

                advance();
                param_i++;
            } while((value_stack.size() - start_param) < num);
        }

        return_();
        return result_t::return_({});
    }
    inline void pop_params(ptr_type num)
    {
        value_stack.erase(value_stack.end() - num, value_stack.end());
    }

    inline scalar deref_real(opcode_layout_t const& from)
    {
        switch(from.exp_type)
        {
        case expression_t::expression:
            return from.to_real();
        case expression_t::global_ref:
            return context.global_by_ptr(from.data_ptr)->short_;
        }

        Throw(undefined_behavior("failed to dereference value"));
    }

    inline i32 deref_i32(opcode_layout_t const& from)
    {
        switch(from.exp_type)
        {
        case expression_t::expression:
            return from.to_u32();
        case expression_t::global_ref:
            return context.global_by_ptr(from.data_ptr)->short_;
        }

        Throw(undefined_behavior("failed to dereference value"));
    }

    inline i16 deref_i16(opcode_layout_t const& from)
    {
        switch(from.exp_type)
        {
        case expression_t::expression:
            return from.to_u16();
        case expression_t::global_ref:
            return context.global_by_ptr(from.data_ptr)->short_;
        }

        Throw(undefined_behavior("failed to dereference value"));
    }

    inline void advance()
    {
        if(current->branching())
        {
            current_ip = current->next_op.ip + 1;
            update_opcode();
        } else
            current_ip = current->next_op.ip;
    }
    inline void update_opcode()
    {
        current = &base[current_ip];
    }
    inline bool finished() const
    {
        return current_ip == terminator;
    }
    inline void call(u16 ip)
    {
        /* Function table points to the last opcode of a function, followd by
         * two opcodes with begin(). The last begin() opcode contains the start
         * of the function */
        jump(base[ip + 2].next_op.ip + 1);
    }
    inline void jump(opcode_layout_t const& opcode)
    {
        auto end = &opcode;

        if((end - base) < 0)
            Throw(undefined_behavior("invalid jump"));

        jump(C_FCAST<ptr_type>(end - base));
    }
    inline void jump(ptr_type ip)
    {
        link_register.push_back(current_ip);
        current_ip = ip;
        update_opcode();
    }
    inline void return_()
    {
        current_ip = link_register.back();
        link_register.pop_back();
        update_opcode();
    }

    inline void restore_state(script_state_t& state)
    {
        link_register = std::move(state.link_register);
        current_ip    = state.ip;
        script_start  = state.script_start;
        script_end    = state.script_end;
        update_opcode();
    }
    inline void stash_state(script_state_t& state)
    {
        state.link_register = std::move(link_register);
        state.ip            = current_ip;
    }

    inline void execute_state(
        script_state_t& state, opcode_handler_t const& handler)
    {
        if(state.is_inactive())
            return;

        restore_state(state);

        eval     run_state = eval::running;
        result_t result;

        while(!finished() && run_state == eval::running)
        {
            result    = evaluate(*current, handler);
            run_state = result.state;
            advance();
        }

        if(run_state == eval::sleeping)
        {
            /* TODO: Set up wait_condition */
            state.status    = script_status::sleeping;
            state.condition = result.condition;
        } else if(state.function->schedule == script_type_t::startup)
        {
            state.status = script_status::finished;
        } else if(state.function->schedule == script_type_t::dormant)
        {
            state.status = script_status::sleeping;
            current_ip   = state.script_start;
        } else
        {
            /* Reset bytecode pointer */
            current_ip = state.script_start;
        }

        stash_state(state);
    }
};

template<typename Bytecode>
struct types
{
    using bytecode_ptr   = bytecode_pointer<Bytecode>;
    using layout_t       = opcode_layout<Bytecode>;
    using opcode_t       = Bytecode;
    using opcode_handler = typename bytecode_ptr::opcode_handler_t;
    using ptr_type       = typename bytecode_ptr::ptr_type;
    using result_t       = typename bytecode_ptr::result_t;

    static inline auto signature(layout_t const& opc)
    {
        return hsc::opcode_signature<Bytecode>(opc);
    }
};

} // namespace hsc
} // namespace blam

namespace Coffee {
namespace Strings {

template<
    typename BC,
    typename std::enable_if<
        std::is_same<BC, blam::hsc::bc::v1>::value ||
        std::is_same<BC, blam::hsc::bc::v2>::value>::type* = nullptr>
inline CString to_string(BC opc)
{
    return blam::hsc::to_string(opc);
}

inline CString to_string(blam::hsc::type_t type)
{
    return blam::hsc::to_string(type);
}

inline CString to_string(blam::hsc::script_eval_result type)
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

inline CString to_string(blam::hsc::script_status stat)
{
    return blam::hsc::to_string(stat);
}

} // namespace Strings
} // namespace Coffee
