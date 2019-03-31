#pragma once

#include <peripherals/libc/types.h>

namespace typing {
namespace pixels {

using namespace libc_types;

template<typename T, u32 min_value = 0, u32 max_value = 0>
struct rgba_untyped
{
    T r, g, b, a;
};

/*!
 * \brief Used for storing typical 8-bit color with alpha
 */
struct rgba_t
{
    rgba_t() : i(0)
    {
    }
    rgba_t(u8 r, u8 g, u8 b, u8 a = 255) : r(r), g(g), b(b), a(a)
    {
    }

    union
    {
        u32 i;
        struct
        {
            u8 r, g, b, a;
        };
    };
    u32 rgba() const
    {
#if defined(COFFEE_LIL_ENDIAN)
        rgba_t t;
        t.r = b;
        t.g = g;
        t.b = r;
        t.a = a;
        return t.i;
#elif defined(COFFEE_BIG_ENDIAN)
        return i;
#endif
    }
};

struct rgb_t
{
    union
    {
        u24 i;
        struct
        {
            u8 r, g, b;
        };
    };
};

} // namespace pixels
} // namespace typing
