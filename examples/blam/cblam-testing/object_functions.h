#pragma once

#include <blam/volta/blam_unit.h>

#include <algorithm>
#include <array>
#include <optional>

namespace blam_functions {

using libc_types::f32;

/* An object function: a constant base, scaled and offset by the inputs
 * (A..D in) and earlier outputs (A..D out) it names */
inline f32 evaluate(
    blam::scn::object::function_t const& fn,
    std::array<f32, 4> const&            in,
    std::array<f32, 4> const&            out)
{
    using src = blam::shader::param_src;
    auto value_of = [&](src s) -> std::optional<f32> {
        auto const i = static_cast<libc_types::u32>(s);
        if(i >= 1 && i <= 4)
            return in[i - 1];
        if(i >= 5 && i <= 8)
            return out[i - 5];
        return std::nullopt;
    };
    f32 v = fn.function == blam::shader::animation_function::zero ? 0.f : 1.f;
    if(auto s = value_of(fn.scale_function_by))
        v *= *s;
    using flags_t = blam::scn::object::function_t::flags_t;
    if((fn.flags & flags_t::invert) != flags_t::none)
        v = 1.f - std::clamp(v, 0.f, 1.f);
    if(auto s = value_of(fn.add))
        v += *s;
    if(auto s = value_of(fn.scale_result_by))
        v *= *s;
    return std::clamp(v, 0.f, 1.f);
}

/* Every function in turn, then off where the one it turns off with is */
template<typename Functions>
inline std::array<f32, 4> evaluate_all(Functions const& functions, std::array<f32, 4> const& in)
{
    std::array<f32, 4> out = in;
    for(size_t i = 0; i < out.size() && i < functions.size(); i++)
        out[i] = evaluate(functions[i], in, out);
    auto const done = out;
    for(size_t i = 0; i < out.size() && i < functions.size(); i++)
        if(auto const off = functions[i].turn_off_with;
           off >= 0 && static_cast<size_t>(off) < done.size() && done[off] <= 0.f)
            out[i] = 0.f;
    return out;
}

} // namespace blam_functions
