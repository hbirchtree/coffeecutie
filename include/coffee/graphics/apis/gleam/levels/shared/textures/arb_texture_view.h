#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_TextureView
{
    STATICINLINE void TexView(CGhnd vh,Texture t,CGhnd th,PixelFormat ifmt,
                        uint32 ml,uint32 nl,uint32 lm,uint32 ln)
    {glTextureView(vh,to_enum(t),th,to_enum(ifmt),ml,nl,lm,ln);}
};
}
}
