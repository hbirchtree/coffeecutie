#pragma once

#include <peripherals/libc/types.h>
#include <random>

namespace stl_types {
namespace math {

using namespace libc_types;

struct rng
{
    using engine = std::mt19937;
    using rng_t  = engine::result_type;

    engine gen;

    template<
        typename T,
        typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    T rand(T min_value, T max_value)
    {
        std::uniform_int_distribution<T> dist(min_value, max_value);
        return dist(gen);
    }

    template<
        typename T,
        typename std::enable_if<std::is_floating_point<T>::value>::type* =
            nullptr>
    T rand(T min_value, T max_value)
    {
        std::uniform_real_distribution<T> dist(min_value, max_value);
        return dist(gen);
    }

    i32 irand()
    {
        using i32_lim = std::numeric_limits<i32>;
        return rand<i32>(i32_lim::min(), i32_lim::max());
    }

    u32 urand()
    {
        using u32_lim = std::numeric_limits<u32>;
        return rand<u32>(u32_lim::min(), u32_lim::min());
    }

    f32 frand()
    {
        return rand<f32>(0.0, 1.0);
    }

    void seed(rng_t seed)
    {
        gen.seed(seed);
    }
};

} // namespace math
} // namespace stl_types
