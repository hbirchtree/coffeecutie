#pragma once

#include <coffee/core/plat/plat_primary_identify.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "rng.h"

namespace Coffee {
/*!
 * \brief Contains functions related to mathematical operations. This namespace
 * was created to simplify overloading the standard functions if there is a need
 * to.
 */
namespace CMath {

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
bool IsPower2(T const& n)
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
T radians(const T& degrees)
{
    return (degrees * CMath::pi) / T(180);
}

template<typename T>
T mix(T a, T b, T f)
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
bool floating_equal(T const& v1, T const& v2)
{
    const auto epsilon = std::numeric_limits<T>::epsilon();

    const auto lower = v1 - epsilon;
    const auto upper = v1 + epsilon;

    return lower < v2 && upper > v2;
}

} // namespace CMath
} // namespace Coffee
