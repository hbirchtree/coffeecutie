#include "blam/volta/hsc/blam_bytecode.h"
#include "blam/volta/blam_base_types.h"
#include "blam/volta/blam_scenario.h"
#include "peripherals/semantic/chunk.h"
#include "peripherals/stl/string/replace.h"
#include <deque>
#include <string>
#define MAGIC_ENUM_RANGE_MIN -4
#define MAGIC_ENUM_RANGE_MAX 550
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

std::string_view to_string(sleep_condition cond)
{
    return magic_enum::enum_name(cond);
}

std::string_view to_string(script_status stat)
{
    return magic_enum::enum_name(stat);
}

template<typename Dialect>
std::string script_to_string(semantic::Span<const opcode_layout<Dialect>> const& bytecode)
{
    std::string script;
    bool last_op{false};
    for(auto const& opcode : bytecode)
    {
        switch(opcode.exp_type)
        {
        case expression_t::group:
            script += fmt::format(" ({}", bc::to_string(opcode.opcode));
            break;
        case expression_t::expression:
            if(opcode.ret_type == type_t::branch_val)
                break;
            switch(opcode.param_type)
            {
            case type_t::long_:
                script += fmt::format(" {}", opcode.to_u32());
                break;
            case type_t::short_:
                script += fmt::format(" {}", opcode.to_u16());
                break;
            case type_t::real_:
                script += fmt::format(" {}", opcode.to_real());
                break;
            case type_t::bool_:
                script += fmt::format(" {}", opcode.to_bool() ? "true" : "false");
                break;
            case type_t::script:
                script += fmt::format(" script({})", opcode.long_);
                break;
            default:
                script += fmt::format(" {}({})", to_string(opcode.param_type), opcode.long_);
                break;
            }
            break;
        case expression_t::global_ref:
            script += fmt::format(" global({})", opcode.long_);
            break;
        case expression_t::script_ref:
            script += fmt::format(" script({})", opcode.long_);
            break;
        default:
            script += " <unknown>";
            break;
        }
        if(opcode.exp_type != expression_t::group && opcode.next_op.ip == 0xFFFF)
        {
            script += ")";
            if(last_op)
            {
                last_op = false;
                script += ")";
            }
        }
        if(opcode.exp_type ==expression_t::group && opcode.next_op.ip == 0xFFFF)
            last_op = true;
    }
    return script;
}

template<typename Version>
std::string to_halo_script(scn::scenario<Version> const& scenario, map_ptr const& magic)
{
    auto scripts = scenario.function_table(magic);
    auto bytecode = scenario.bytecode(magic);
    std::string script;
    for(auto const& decl : scripts)
    {
        auto definition = bytecode.subspan(decl.index + 1, 2);
        auto start      = definition[1].next_op.ip + 1;
        auto start_op   = bytecode[start].index;
        auto length     = definition[0].index - start_op;
        auto body       = bytecode.subspan(start, length);
        script += fmt::format(
            "(script[{}] {} {}\n{}\n)\n\n",
            to_string(decl.type),
            decl.name.str(),
            to_string(decl.schedule),
            script_to_string(body));
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

