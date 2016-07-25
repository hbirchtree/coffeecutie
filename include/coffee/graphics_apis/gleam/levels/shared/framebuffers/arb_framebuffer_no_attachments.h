#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_FramebufferNoAttachments
{
    STATICINLINE void FBParameteri(CGenum t,CGenum p,int32 d)
    {glFramebufferParameteri(t,p,d);}
    STATICINLINE void FBGetParameteriv(FramebufferT t, CGenum p, int32* v)
    {glGetFramebufferParameteriv(to_enum(t),p,v);}
};
}
}
