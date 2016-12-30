#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_TextureStorage
{
    STATICINLINE bool TexStorage2D(Texture t,uint32 l,PixelFormat ifmt,uint32 w,uint32 h)
    {
        if(!glTexStorage2D)
            return false;
        glTexStorage2D(to_enum(t),l,to_enum(ifmt),w,h);
        //TODO: Add verification in debug mode
        return true;
    }
    STATICINLINE bool TexStorage3D(Texture t,uint32 l,PixelFormat ifmt,uint32 w,uint32 h, uint32 d)
    {
        if(!glTexStorage3D)
            return false;
        glTexStorage3D(to_enum(t),l,to_enum(ifmt),w,h,d);
        //TODO: Add verification in debug mode
        return true;
    }
};
}
}
