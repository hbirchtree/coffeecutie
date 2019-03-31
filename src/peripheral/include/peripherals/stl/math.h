#pragma once

#include <peripherals/identify/compiler.h>
#include <peripherals/identify/system.h>

#include <peripherals/libc/types.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#include "rng.h"

namespace stl_types {
namespace math {

using namespace libc_types;
using namespace std;

#ifdef COFFEE_WINDOWS
template<typename T>
T min(T v1, T v2)
{
    return std::min(v1, v2);
}

template<typename T>
T max(T v1, T v2)
{
    return std::max(v1, v2);
}
#endif

template<typename T>
inline bool IsPower2(T const& n)
{
    return n != 0 && (n & (n - 1)) == 0;
}

#if !defined(M_PI)
constexpr bigscalar pi = 3.1416;
#else
constexpr bigscalar pi = M_PI;
#endif
#if !defined(M_E)
constexpr bigscalar e = 2.7183;
#else
constexpr bigscalar e  = M_E;
#endif

template<typename T>
inline T radians(const T& degrees)
{
    return (degrees * math::pi) / T(180);
}

template<typename T>
inline T mix(T a, T b, T f)
{
    return a * (T(1) - f) + b * f;
}

template<
    typename T,
    typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
/*!
 * \brief Check if values are within epsilon limits for the given type
 * This is only necessary for floating-point types.
 *
 * \param v1
 * \param v2
 * \return
 */
FORCEDINLINE bool floating_equal(T const& v1, T const& v2)
{
    if(v1 == v2)
        return true;

    const auto epsilon = std::numeric_limits<T>::epsilon();

    const auto lower = v1 - epsilon * 10;
    const auto upper = v1 + epsilon * 10;

    return lower < v2 && upper > v2;
}

} // namespace math
} // namespace stl_types
