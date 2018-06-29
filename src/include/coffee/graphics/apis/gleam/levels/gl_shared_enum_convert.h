#pragma once

#include "gl_shared_types.h"
#include <coffee/core/types/edef/logicenum.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee {
namespace CGL {

struct DrwMd
{
    Prim    t;
    PrimCre c;
};

FORCEDINLINE CGenum to_enum(PatchProperty f)
{
    return C_CAST<CGenum>(f);
}

CGenum to_enum(Feature e, u32 offset = 0);

CGenum to_enum(Prim p, PrimCre c);

FORCEDINLINE CGenum to_enum(DrwMd m)
{
    return to_enum(m.t, m.c);
}

CGenum to_enum(
    PixelFormat f,
    PixelFlags  e = PixelFlags::None,
    CompFlags   d = CompFlags::CompressionNone);

CGenum to_enum(Filtering p, Filtering s = Filtering::None);

} // namespace CGL
} // namespace Coffee

#include "gl_to_enum.inl"
