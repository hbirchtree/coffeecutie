#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{
namespace CGL{

template<typename CGhnd,typename BufType, typename ReqVer>
struct CGL_Old_Buffers
{
    GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
    STATICINLINE void BufBind(BufType t,CGhnd h)
    {
        glBindBuffer(to_enum(t),h);
    }

    GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
    STATICINLINE void BufData(BufType t,Bytes const& data,RSCA a)
    {
        glBufferData(to_enum(t),data.size,data.data,to_enum1(a));
    }

    GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
    STATICINLINE void BufSubData(BufType t,int64 off,Bytes const& data)
    {
        glBufferSubData(to_enum(t),off,data.size,data.data);
    }

    GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
    STATICINLINE void BufCopySubData(BufType t1,BufType t2,
                                     szptr off1,szptr off2,szptr sz)
    {
        glCopyBufferSubData(to_enum(t1),to_enum(t2),off1,off2,sz);
    }

#if !defined(COFFEE_ONLY_GLES20)
    GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
    STATICINLINE void BufBindRange(BufType t,uint32 i,CGhnd b,szptr off,szptr sz)
    {
        glBindBufferRange(to_enum(t),i,b,off,sz);
    }

    GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
    STATICINLINE void* BufMapRange(BufType t,szptr off,szptr sz,ResourceAccess a)
    {
        return glMapBufferRange(to_enum(t),off,sz,to_enum2(a));
    }

    GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
    STATICINLINE void BufUnmap(BufType t)
    {
        glUnmapBuffer(to_enum(t));
    }
#endif

    GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
    STATICINLINE void BufGetParameteriv(BufType t, CGenum p, i32* v)
    {
        glGetBufferParameteriv(to_enum(t),p,v);
    }
#if !defined(COFFEE_ONLY_GLES20)
    GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
    STATICINLINE void BufGetParameterLLv(BufType t, CGenum p, i64* v)
    {glGetBufferParameteri64v(to_enum(t),p,v);}
#endif
};

}
}
