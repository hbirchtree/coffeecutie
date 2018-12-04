#pragma once

#include "gl_shared_types.h"
#include <coffee/core/types/filtering.h>
#include <coffee/core/types/pixel_format.h>
#include <peripherals/typing/enum/graphics/primitive.h>

namespace Coffee {
namespace CGL {

using Prim    = typing::graphics::primitive::base;
using PrimCre = typing::graphics::primitive::creation;

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
    PixFmt    f,
    PixFlg    e = PixFlg::None,
    CompFlags d = CompFlags::CompressionNone);

CGenum to_enum(Filtering p, Filtering s = Filtering::None);

} // namespace CGL
} // namespace Coffee

#include "gl_to_enum.inl"
