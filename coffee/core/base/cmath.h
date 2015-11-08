#ifndef CMATH_FUN
#define CMATH_FUN

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

#include "cmath_glm.h"

namespace Coffee{
/*!
 * \brief Contains functions related to mathematical operations. This namespace was created to simplify overloading the standard functions if there is a need to.
 */
namespace CMath{
using namespace std;

template<typename T>
inline static bool power2(T n)
{
    return n!=0&&(n&(n-1))==0;
}

static constexpr bigscalar pi = M_PI;
static constexpr bigscalar e = M_E;

template<typename T>
inline static T max(T a, T b)
{
    return (a>b) ? a : b;
}

template<typename T>
inline static T min(T a, T b)
{
    return (a<b) ? a : b;
}

/*!
 * \brief Random number generator, static variable for use in crand()
 */
static thread_local std::mt19937 rand_gen;

inline static void csrand(uint64 seed)
{
    rand_gen.seed(seed);
}
inline static int crand()
{
    return rand_gen();
}
inline static int crand(const int& min, const int& max)
{
    uniform_int_distribution<int> dist(min,max);
    return dist(rand_gen);
}
inline static scalar cfrand(const scalar& min, const scalar& max)
{
    uniform_real_distribution<scalar> dist(min,max);
    return dist(rand_gen);
}

}
}

#endif
