#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_TextureStorageMultisample
{
    STATICINLINE bool TexStorage2DMS(Texture t,uint32 samples,uint32 l,
                               PixelFormat ifmt,uint32 w,uint32 h)
    {
        if(!glTexStorage2DMultisample)
            return false;
        glTexStorage2DMultisample(to_enum(t),samples,l,to_enum(ifmt),w,h);
        //TODO: Add verification in debug mode
        return true;
    }
    STATICINLINE bool TexStorage3DMS(Texture t,uint32 samples,uint32 l,
                               PixelFormat ifmt,uint32 w,uint32 h, uint32 d)
    {
        if(!glTexStorage3DMultisample)
            return false;
        glTexStorage3DMultisample(to_enum(t),samples,l,to_enum(ifmt),w,h,d);
        //TODO: Add verification in debug mode
        return true;
    }
};
}
}
