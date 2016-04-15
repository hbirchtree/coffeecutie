#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_TessellationShader
{
    /* Tessellation */
    STATICINLINE void PatchParameteri(PatchProperty p,int32 v)
    {glPatchParameteri(to_enum(p),v);}
    STATICINLINE void PatchParameterfv(PatchProperty p,const scalar* v)
    {glPatchParameterfv(to_enum(p),v);}
};
}
}
