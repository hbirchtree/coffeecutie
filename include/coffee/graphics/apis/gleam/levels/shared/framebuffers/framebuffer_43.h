#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_Framebuffers_43
{
    STATICINLINE void FBAttachTexture(FramebufferT t,CGenum att,CGhnd h,int32 level)
    {glFramebufferTexture(to_enum(t),att,h,level);}
};
}
}
