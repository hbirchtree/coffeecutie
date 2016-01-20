#ifndef CMATH_FUN
#define CMATH_FUN

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

#include <types/basetypes.h>

namespace Coffee{
/*!
 * \brief Contains functions related to mathematical operations. This namespace was created to simplify overloading the standard functions if there is a need to.
 */
namespace CMath{
using namespace std;

template<typename T> bool power2(T const& n)
{
    return n!=0&&(n&(n-1))==0;
}

constexpr bigscalar pi = M_PI;
constexpr bigscalar e = M_E;

/*
template<typename T> T max(T const& v1, T const& v2)
{
    return (v1>v2) ? v1 : v2;
}

template<typename T> T min(T const& v1, T const& v2)
{
    return (v1<v2) ? v1 : v2;
}

**!
 * \brief Random number generator, static variable for use in crand()
*
static thread_local std::mt19937 rand_gen;

inline void csrand(uint64 seed)
{
    rand_gen.seed(seed);
}
inline int crand()
{
    return rand_gen();
}
inline int crand(const int& min, const int& max)
{
    uniform_int_distribution<int> dist(min,max);
    return dist(rand_gen);
}
inline scalar cfrand(const scalar& min, const scalar& max)
{
    uniform_real_distribution<scalar> dist(min,max);
    return dist(rand_gen);
}
*/

template<typename T> T radians(const T& degrees)
{
    return degrees*CMath::pi/T(180);
}

template<typename T> T mix(T a, T b, T f)
{
    return a*(T(1)-f)+b*f;
}

}
}

#endif
