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

} // namespace CMath
} // namespace Coffee
