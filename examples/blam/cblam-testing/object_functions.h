#pragma once

#include <blam/volta/blam_unit.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>

namespace blam_functions {

using libc_types::f32;

/* A periodic function `time` seconds in, 0..1; the ones of no fixed shape
 * hold at 1 */
inline f32 periodic(blam::shader::animation_function function, f32 period, f32 time)
{
    using fn_t = blam::shader::animation_function;
    if(function == fn_t::zero)
        return 0.f;
    if(function == fn_t::one || period <= 0.f)
        return 1.f;
    f32 const phase = time / period - std::floor(time / period);
    switch(function)
    {
    case fn_t::cosine:
    case fn_t::cosine_variable:
        return .5f + .5f * std::cos(phase * 6.2831853f);
    case fn_t::diagonal:
    case fn_t::diagonal_period:
        return 1.f - std::abs(2.f * phase - 1.f);
    case fn_t::slide:
    case fn_t::slide_variable:
        return phase;
    default:
        return 1.f;
    }
}

/* An object function: a constant base, scaled and offset by the inputs
 * (A..D in) and earlier outputs (A..D out) it names. Without a `time` the
 * periodic ones hold at 1 */
inline f32 evaluate(
    blam::scn::object::function_t const& fn,
    std::array<f32, 4> const&            in,
    std::array<f32, 4> const&            out,
    std::optional<f32>                   time = std::nullopt)
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
    f32 v = time ? periodic(fn.function, fn.period, *time)
            : fn.function == blam::shader::animation_function::zero ? 0.f
                                                                     : 1.f;
    if(auto s = value_of(fn.scale_function_by))
        v *= *s;
    using flags_t = blam::scn::object::function_t::flags_t;
    if((fn.flags & flags_t::invert) != flags_t::none)
        v = 1.f - std::clamp(v, 0.f, 1.f);
    if(auto s = value_of(fn.add))
        v += *s;
    if(auto s = value_of(fn.scale_result_by))
        v *= *s;
    /* Bounds other than 0..1 map or clip the result; an additive function's
     * result is the step it adds each tick */
    using mode_t = blam::scn::object::function_t::bounds_mode_t;
    if(f32 const lo = fn.bounds.x, hi = fn.bounds.y; hi > lo && (lo != 0.f || hi != 1.f))
        switch(fn.bounds_mode)
        {
        case mode_t::scale_to_fit:
            v = lo + std::clamp(v, 0.f, 1.f) * (hi - lo);
            break;
        case mode_t::clip_and_normalize:
            v = (std::clamp(v, lo, hi) - lo) / (hi - lo);
            break;
        default:
            v = std::clamp(v, lo, hi);
            break;
        }
    /* A last factor; 0 leaves it be (the cursed banshee's flap step) */
    if(fn.scale_by > 0.f)
        v *= fn.scale_by;
    return std::clamp(v, 0.f, 1.f);
}

inline bool additive(blam::scn::object::function_t const& fn)
{
    using flags_t = blam::scn::object::function_t::flags_t;
    return (fn.flags & flags_t::additive) != flags_t::none;
}

/* Every function in turn, then off where the one it turns off with is */
template<typename Functions>
inline std::array<f32, 4> evaluate_all(
    Functions const& functions, std::array<f32, 4> const& in, std::optional<f32> time = std::nullopt)
{
    std::array<f32, 4> out = in;
    for(size_t i = 0; i < out.size() && i < functions.size(); i++)
        out[i] = evaluate(functions[i], in, out, time);
    auto const done = out;
    for(size_t i = 0; i < out.size() && i < functions.size(); i++)
        if(auto const off = functions[i].turn_off_with;
           off >= 0 && static_cast<size_t>(off) < done.size() && done[off] <= 0.f)
            out[i] = 0.f;
    return out;
}

} // namespace blam_functions
