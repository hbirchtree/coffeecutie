#include "blam/volta/hsc/blam_bytecode.h"
#include "blam/volta/blam_base_types.h"
#include "blam/volta/blam_scenario.h"
#include "peripherals/semantic/chunk.h"
#include "peripherals/stl/string/replace.h"
#include <deque>
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

}

