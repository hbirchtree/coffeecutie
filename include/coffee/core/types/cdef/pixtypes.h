#pragma once

#include "../../plat/memory/cmemory.h"
#include "../tdef/integertypes.h"
#include "../tdef/stltypes.h"
#include "geometry.h"

namespace Coffee{

/*!
 * \brief Used for storing typical 8-bit color with alpha
 */
struct CRGBA
{
    CRGBA():
        i(0)
    {
    }
    CRGBA(uint8 r,uint8 g,uint8 b,uint8 a = 255):
        r(r),g(g),b(b),a(a)
    {
    }

    union{
        uint32 i;
        struct
        {
            uint8 r,g,b,a;
        };
    };
    uint32 rgba() const
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
            uint8 r,g,b;
        };
    };
};

template<typename PixelType,typename DimT>
struct _cbasic_bitmap_base
{
    FORCEDINLINE _cbasic_bitmap_base(DimT w, DimT h):
        size(w,h)
    {
        m_pixels = new PixelType[sizeof(PixelType),w*h];
        m_internal_data = true;
    }
    FORCEDINLINE _cbasic_bitmap_base(DimT w, DimT h, PixelType* data):
        size(w,h)
    {
        m_pixels = data;
        m_internal_data = false;
    }

    FORCEDINLINE ~_cbasic_bitmap_base()
    {
        if(m_internal_data)
            delete[] m_pixels;
    }

    const _cbasic_size_2d<DimT> size;

    FORCEDINLINE PixelType* data()
    {
        return m_pixels;
    }
    FORCEDINLINE const PixelType* data() const
    {
        return m_pixels;
    }
private:
    PixelType* m_pixels;
    bool m_internal_data;
};

struct CColorMask
{
    bool r:1;
    bool g:1;
    bool b:1;
    bool a:1;
    uint8 padding:4;
};

using CBitmap = _cbasic_bitmap_base<CRGBA,int32>;
using Bitmap = CBitmap;

}
