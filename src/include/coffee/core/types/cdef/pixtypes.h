#pragma once

#include "../tdef/integertypes.h"
#include "../tdef/stltypes.h"
#include "geometry.h"

namespace Coffee{

template<typename T, u32 min_value = 0, u32 max_value = 0>
struct rgba_untyped
{
    T r, g, b, a;
};

/*!
 * \brief Used for storing typical 8-bit color with alpha
 */
struct CRGBA
{
    CRGBA():
        i(0)
    {
    }
    CRGBA(u8 r,u8 g,u8 b,u8 a = 255):
        r(r),g(g),b(b),a(a)
    {
    }

    union{
        u32 i;
        struct
        {
            u8 r,g,b,a;
        };
    };
    u32 rgba() const
    {
#if defined(COFFEE_LIL_ENDIAN)
        CRGBA t;
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

struct CRGB
{
    union{
        uint24 i;
        struct
        {
            u8 r,g,b;
        };
    };
};

struct CColorMask
{
    bool r:1;
    bool g:1;
    bool b:1;
    bool a:1;
    u8 padding:4;
};

using rgba_t = CRGBA;
using rgb_t = CRGB;
using colormask_t = CColorMask;

}
