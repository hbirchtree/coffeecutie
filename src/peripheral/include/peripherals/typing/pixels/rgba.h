#pragma once

#include <peripherals/libc/types.h>

namespace typing {
namespace pixels {

template<
    typename T,
    libc_types::u32 min_value = 0,
    libc_types::u32 max_value = 0>
struct rgba_untyped
{
    T r, g, b, a;
};

/*!
 * \brief Used for storing typical 8-bit color with alpha
 */
union rgba_t
{
    rgba_t() : i(0)
    {
    }
    rgba_t(
        libc_types::u8 r,
        libc_types::u8 g,
        libc_types::u8 b,
        libc_types::u8 a = 255) :
        r(r), g(g), b(b), a(a)
    {
    }

    libc_types::u32 rgba() const
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

    libc_types::u32 i;
    struct
    {
        u8 r, g, b, a;
    };
};

union rgb_t
{
    libc_types::u24 i;
    struct
    {
        libc_types::u8 r, g, b;
    };
};

} // namespace pixels
} // namespace typing
