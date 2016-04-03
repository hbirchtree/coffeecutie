#pragma once

#include "gl_shared_types.h"
#include <coffee/core/types/edef/logicenum.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

#ifdef COFFEE_GLEAM_DESKTOP
FORCEDINLINE CGenum to_enum(PatchProperty f)
{
    return (CGenum)f;
}
#endif

FORCEDINLINE CGenum to_enum(Feature e);
FORCEDINLINE CGenum to_enum(QueryT f);

FORCEDINLINE CGenum to_enum(Severity s);
FORCEDINLINE CGenum to_enum(DebugType t);
FORCEDINLINE CGenum to_enum(Object t);
FORCEDINLINE CGenum to_enum(Face f);
FORCEDINLINE CGenum to_enum(BufBit f);
FORCEDINLINE CGenum to_enum(Prim p,PrimCre c);
FORCEDINLINE CGenum to_enum(bool pack, PixelOperation f);
FORCEDINLINE CGenum to_enum(PixelFormat f, PixelFlags e = PixelFlags::None,
                            CompFlags d = CompressionNone);
/*!
     * \brief Returns GL_*_SHADER enum
     * \param f
     * \return
     */
FORCEDINLINE CGenum to_enum1(ShaderStage f);
/*!
     * \brief Returns GL_*_SHADER_BIT enum
     * \param f
     * \return
     */
FORCEDINLINE CGenum to_enum2(ShaderStage f);
FORCEDINLINE CGenum to_enum(ValueHandling f);
FORCEDINLINE CGenum to_enum(ValueComparison f);
FORCEDINLINE CGenum to_enum(Operator f);
FORCEDINLINE CGenum to_enum(LogicOp op);

FORCEDINLINE CGenum to_enum1(ResourceAccess acc);
FORCEDINLINE CGenum to_enum2(ResourceAccess acc);
FORCEDINLINE CGenum to_enum3(ResourceAccess acc);

FORCEDINLINE CGenum to_enum(PixelComponents f);
FORCEDINLINE CGenum to_enum(TypeEnum f);
FORCEDINLINE CGenum to_enum(BitFormat f);

FORCEDINLINE CGenum to_enum(FramebufferT f);

FORCEDINLINE CGenum to_enum(AttribMode f);

FORCEDINLINE CGpixfmt get_fmt(PixelFormat e, bool rev);

}
}

#include "../gl_to_enum.inl"
