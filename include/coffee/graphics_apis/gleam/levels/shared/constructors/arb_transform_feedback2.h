#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_XF2_Allocators
{
    STATICINLINE bool XFAlloc(uint32 l,CGhnd* d){glGenTransformFeedbacks(l,d); return true;}
    STATICINLINE bool XFFree(uint32 l,CGhnd* d){glDeleteTransformFeedbacks(l,d); return true;}
};

}
}
