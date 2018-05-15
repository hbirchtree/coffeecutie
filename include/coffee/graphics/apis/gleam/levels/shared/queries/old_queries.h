#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace CGL{

template<typename QueryT,typename CGhnd,typename CGenum>
struct CGL_Old_Queries
{
    /* Query actions */
    STATICINLINE void QueryBegin(QueryT t,CGhnd h)
    {glBeginQuery(to_enum(t),h);}
    STATICINLINE void QueryEnd(QueryT t)
    {glEndQuery(to_enum(t));}

    /* GetQuery*v */
    STATICINLINE void QueryGetiv(QueryT t, CGenum p, int32* v)
    {glGetQueryiv(to_enum(t),p,v);}

    /* GetQueryObject*v */
#if GL_VERSION_VERIFY(0x200, 0x300)
    STATICINLINE void QueryGetObjectuiv(CGhnd h,CGenum f,uint32* d)
    {glGetQueryObjectuiv(h,f,d);}
#endif

#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
    STATICINLINE void QueryGetObjectiv(CGhnd h,CGenum f,int32* d)
    {glGetQueryObjectiv(h,f,d);}
#endif
#if GL_VERSION_VERIFY(0x320, GL_VERSION_NONE)
    STATICINLINE void QueryGetObjecti64v(CGhnd h,CGenum f,int64* d)
    {glGetQueryObjecti64v(h,f,d);}
    STATICINLINE void QueryGetObjectui64v(CGhnd h,CGenum f,uint64* d)
    {glGetQueryObjectui64v(h,f,d);}
#endif
};

}
}
