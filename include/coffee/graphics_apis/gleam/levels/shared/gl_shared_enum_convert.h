#pragma once

#include "gl_shared_types.h"
#include <coffee/core/types/edef/logicenum.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct DrwMd
{
    Prim t;
    PrimCre c;
};

#ifdef COFFEE_GLEAM_DESKTOP
FORCEDINLINE CGenum to_enum(PatchProperty f)
{
    return (CGenum)f;
}
#endif

CGenum to_enum(Feature e);
CGenum to_enum(QueryT f);

CGenum to_enum(Severity s);
CGenum to_enum(DebugType t);
CGenum to_enum(Object t);
CGenum to_enum(Face f);
CGenum to_enum(BufBit f);
CGenum to_enum(Prim p,PrimCre c);
FORCEDINLINE CGenum to_enum(DrwMd m)
{
    return to_enum(m.t,m.c);
}
CGenum to_enum(bool pack, PixelOperation f);
CGenum to_enum(PixelFormat f, PixelFlags e = PixelFlags::None,
                            CompFlags d = CompressionNone);
/*!
     * \brief Returns GL_*_SHADER enum
     * \param f
     * \return
     */
CGenum to_enum1(ShaderStage f);
/*!
     * \brief Returns GL_*_SHADER_BIT enum
     * \param f
     * \return
     */
CGenum to_enum2(ShaderStage f);
CGenum to_enum(ValueHandling f);
CGenum to_enum(ValueComparison f);
CGenum to_enum(Operator f);
CGenum to_enum(LogicOp op);

CGenum to_enum1(ResourceAccess acc);
CGenum to_enum2(ResourceAccess acc);
CGenum to_enum3(ResourceAccess acc);

CGenum to_enum(PixelComponents f);
CGenum to_enum(TypeEnum f);
CGenum to_enum(BitFormat f);

CGenum to_enum(FramebufferT f);

CGenum to_enum(AttribMode f);

CGpixfmt get_fmt(PixelFormat e, bool rev);

}
}

#include "../gl_to_enum.inl"
