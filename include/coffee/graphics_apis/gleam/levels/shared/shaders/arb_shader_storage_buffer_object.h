#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_SSBO
{
    /* Shader buffer */
    STATICINLINE void SBufBind(CGhnd h,uint32 i,uint32 b)
    {glShaderStorageBlockBinding(h,i,b);}
};
}
}
