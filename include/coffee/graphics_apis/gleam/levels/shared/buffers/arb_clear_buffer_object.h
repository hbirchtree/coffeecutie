#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_ClearBuffer
{
    /* Buffers */
    STATICINLINE void BufClearData(BufType t,PixelFormat ifmt,TypeEnum dtb,
                                   TypeEnum dts,c_cptr d)
    {glClearBufferData(to_enum(t),to_enum(ifmt),to_enum(dtb),to_enum(dts),d);}
    STATICINLINE void BufClearSubData(BufType t,PixelFormat ifmt,int64 offset,uint32 size,
                                      PixelFormat df,TypeEnum dt,c_cptr d)
    {glClearBufferSubData(to_enum(t),to_enum(ifmt),offset,size,to_enum(df),to_enum(dt),d);}
};
}
}
