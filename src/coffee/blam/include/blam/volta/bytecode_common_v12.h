#pragma once

#include "cblam_bytecode_opcodes.h"
#include "cblam_structures.h"
#include <peripherals/stl/time_types.h>

#define MAGIC_ENUM_RANGE_MIN -10
#define MAGIC_ENUM_RANGE_MAX 550
#include <peripherals/stl/magic_enum.hpp>

namespace blam::hsc {

struct script_error : undefined_behavior
{
    using undefined_behavior::undefined_behavior;
};

struct missing_signature : script_error
{
    using script_error::script_error;
};

struct mismatch_param_type : script_error
{
    using script_error::script_error;
};

template<typename BC>
struct script_ref;

constexpr u16 terminator = std::numeric_limits<u16>::max();

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
    branch_val  = 2,
    passthrough = 3,
    void_       = 4,
    bool_       = 5,
    real_       = 6,
    short_      = 7,
    long_       = 8,
    string_     = 9,

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

    /* Object types */
    object = 37,
    unit,
    vehicle,
    weapon,
    device,
    scenery,

    /* Strings referring to object types */
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
requires is_bytecode_variant<Bytecode>
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
        f32           real;
        i32           long_;
        Array<i16, 2> shorts;
        Array<u8, 4>  bytes;
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

    inline bool to_bool() const
    {
        verify_expression();

        if(param_type != type_t::bool_)
            Throw(undefined_behavior("invalid bool"));

        return static_cast<bool>(bytes[0]);
    }
    inline i16 to_u16() const
    {
        verify_expression();

        if(param_type != type_t::short_)
            Throw(undefined_behavior("invalid u16"));

        return shorts[0];
    }
    inline i32 to_u32() const
    {
        verify_expression();

        if(param_type != type_t::long_ && param_type != type_t::short_)
            Throw(undefined_behavior("invalid u32"));

        return long_;
    }
    inline f32 to_real() const
    {
        verify_expression();

        if(param_type != type_t::real_)
            Throw(undefined_behavior("invalid real"));

        return real;
    }
    inline u32 to_ptr() const
    {
        return data_ptr;
    }
    inline std::string_view to_str(string_segment_ref const& string_seg) const
    {
        verify_expression();

        if(param_type != type_t::string_)
            Throw(undefined_behavior("invalid string"));

        return string_seg.at(data_ptr).data;
    }

    template<
        typename T,
        typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
    inline void set(T v)
    {
        long_    = 0;
        bytes[0] = v;
    }
    inline void set(i16 v)
    {
        shorts[0] = v;
        shorts[1] = 0;
    }
    inline void set(i32 v)
    {
        long_ = v;
    }
    inline void set(f32 v)
    {
        real = v;
    }
    inline void set_ptr(u32 ptr)
    {
        data_ptr = ptr;
    }

    template<typename T>
    inline T get(u32 off = 0) const
    {
        /* TODO: Fix this with concepts */
        if constexpr(std::is_same_v<T, u8>)
            return bytes.at(off);
        else if constexpr(std::is_same_v<T, i16>)
            return shorts.at(off);
        else if constexpr(std::is_same_v<T, u16>)
            return static_cast<u16>(shorts.at(off));
        else if constexpr(std::is_same_v<T, i32>)
            return long_;
        else if constexpr(std::is_same_v<T, f32>)
            return real;
        else
            Throw(script_error("invalid type from opcode"));
    }

    /* Value types for return */
    static inline opcode_layout void_()
    {
        opcode_layout out;
        out.ret_type = out.param_type = type_t::void_;
        out.exp_type                  = expression_t::expression;
        out.opcode                    = Bytecode::invalid;

        out.next_op.ip   = terminator;
        out.next_op.salt = terminator;

        out.set_ptr(0);
        out.set(0);
        return out;
    }
    static inline opcode_layout typed_(type_t rtype)
    {
        opcode_layout out = void_();
        out.ret_type = out.param_type = rtype;
        return out;
    }

    inline opcode_layout apply_operator(
        opcode_layout const&                      other,
        stl_types::Function<f32(f32, f32)> const& op)
    {
        auto cpy = *this;

        switch(other.param_type)
        {
        case type_t::short_:
        case type_t::long_: {
            switch(param_type)
            {
            case type_t::short_:
            case type_t::long_: {
                cpy.long_ = op(cpy.long_, other.long_);
                break;
            }
            case type_t::real_: {
                cpy.real = op(cpy.real, other.long_);
                break;
            }
            }
            break;
        }
        case type_t::real_: {
            switch(param_type)
            {
            case type_t::short_:
            case type_t::long_: {
                cpy.long_ = op(cpy.long_, other.real);
                break;
            }
            case type_t::real_: {
                cpy.real = op(cpy.real, other.real);
                break;
            }
            }
            break;
        }
        }

        return cpy;
    }

    inline opcode_layout operator+(opcode_layout const& other)
    {
        return apply_operator(
            other, [](auto op1, auto op2) { return op1 + op2; });
    }

    inline opcode_layout operator-(opcode_layout const& other)
    {
        return apply_operator(
            other, [](auto op1, auto op2) { return op1 - op2; });
    }

    inline opcode_layout operator*(opcode_layout const& other)
    {
        return apply_operator(
            other, [](auto op1, auto op2) { return op1 * op2; });
    }

    inline opcode_layout operator/(opcode_layout const& other)
    {
        return apply_operator(
            other, [](auto op1, auto op2) { return op1 / op2; });
    }
};

} // namespace blam::hsc
