#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_ShaderImageLoadStore
{
    STATICINLINE void MemoryBarrier(GLbitfield f){glMemoryBarrier(f);}
    STATICINLINE void ImageBindTexture(int32 tx,CGhnd t,uint32 lev,bool lay,
                                 uint32 ln,ResourceAccess acc,PixelFormat fmt)
    {glBindImageTexture(tx,t,lev,lay,ln,to_enum3(acc),to_enum(fmt));}
};
}
}
