#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_XF3
{
    /* Queries */
    STATICINLINE void QueryIndexedBegin(QueryT t,uint32 i,CGhnd h)
    {glBeginQueryIndexed(to_enum(t),i,h);}
    STATICINLINE void QueryIndexedEnd(QueryT t,uint32 i){glEndQueryIndexed(to_enum(t),i);}
    //TODO: Create QueryProperty enum
    STATICINLINE void QueryIndexedGetiv(QueryT t,uint32 i,CGenum e,int32* v)
    {glGetQueryIndexediv(to_enum(t),i,e,v);}
};
}
}
