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
signatures::sig_t opcode_signature(opcode_layout<Bytecode> const& op);

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
