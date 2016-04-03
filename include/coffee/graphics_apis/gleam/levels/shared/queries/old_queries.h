#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace CGL{

template<typename QueryT,typename CGhnd,typename CGenum>
struct CGL_Old_Queries
{
    STATICINLINE void QueryBegin(QueryT t,CGhnd h)
    {glBeginQuery(to_enum(t),h);}
    STATICINLINE void QueryEnd(QueryT t)
    {glEndQuery(to_enum(t));}

    STATICINLINE void QueryGetObjectuiv(CGhnd h,CGenum f,uint32* d)
    {glGetQueryObjectuiv(h,f,d);}
};

}
}
