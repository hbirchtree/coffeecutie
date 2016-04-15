#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_CopyImage
{
    STATICINLINE bool ImageCopySubData(CGhnd s,Texture st,uint32 sl,int32 sx,int32 sy,int32 sz,
                                 CGhnd d,Texture dt,uint32 dl,int32 dx,int32 dy,int32 dz,
                                 uint32 sw,uint32 sh,uint32 sd)
    {
        glCopyImageSubData(s,to_enum(st),sl,sx,sy,sz,d,to_enum(dt),dl,dx,dy,dz,sw,sh,sd);
        return true;
    }
};
}
}
