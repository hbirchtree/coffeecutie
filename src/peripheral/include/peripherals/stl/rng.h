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
        return rand<i32>(Int32_Min, Int32_Max);
    }
    u32 urand()
    {
        return rand<u32>(0, UInt32_Max);
    }

    scalar frand()
    {
        return rand<scalar>(0.0, 1.0);
    }

    void seed(rng_t seed)
    {
        gen.seed(seed);
    }
};

} // namespace math
} // namespace stl_types
