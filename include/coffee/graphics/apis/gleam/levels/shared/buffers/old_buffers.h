#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

template<typename CGhnd,typename BufType>
struct CGL_Old_Buffers
{

    STATICINLINE void BufBind(BufType t,CGhnd h)
    {glBindBuffer(to_enum(t),h);}

    STATICINLINE void BufData(BufType t,uint64 sz,c_cptr p,ResourceAccess a)
    {glBufferData(to_enum(t),sz,p,to_enum1(a));}

    STATICINLINE void BufSubData(BufType t,int64 off,uint32 sz,c_cptr p)
    {glBufferSubData(to_enum(t),off,sz,p);}

    STATICINLINE void BufCopySubData(BufType t1,BufType t2,szptr off1,szptr off2,szptr sz)
    {glCopyBufferSubData(to_enum(t1),to_enum(t2),off1,off2,sz);}

#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void BufBindRange(BufType t,uint32 i,CGhnd b,szptr off,szptr sz)
    {glBindBufferRange(to_enum(t),i,b,off,sz);}
    STATICINLINE void* BufMapRange(BufType t,szptr off,szptr sz,ResourceAccess a)
    {return glMapBufferRange(to_enum(t),off,sz,to_enum2(a));}
    STATICINLINE void BufUnmap(BufType t)
    {glUnmapBuffer(to_enum(t));}
#endif

    STATICINLINE void BufGetParameteriv(BufType t, CGenum p, int32* v)
    {glGetBufferParameteriv(to_enum(t),p,v);}
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void BufGetParameterLLv(BufType t, CGenum p, int64* v)
    {glGetBufferParameteri64v(to_enum(t),p,v);}
#endif
};

}
}
