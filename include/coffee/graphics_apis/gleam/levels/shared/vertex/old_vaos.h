#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace CGL{

template<typename CGhnd,typename CGenum>
struct CGL_Old_VAOs
{
    STATICINLINE void VAOBind(CGhnd h){glBindVertexArray(h);}

    STATICINLINE void VAOEnableAttrib(uint32 i)
    {glEnableVertexAttribArray(i);}
    STATICINLINE void VAOAttribPointer(uint32 i,uint32 s,TypeEnum t,bool n,int64 stride,int64 offset)
    {glVertexAttribPointer(i,s,to_enum(t),(n) ? GL_TRUE : GL_FALSE,stride,(void*)offset);}
    STATICINLINE void VAOAttribIPointer(uint32 i,uint32 s,TypeEnum t,int64 stride,int64 offset)
    {glVertexAttribIPointer(i,s,to_enum(t),stride,(void*)offset);}
    STATICINLINE void VAODivisor(uint32 i,uint32 d)
    {glVertexAttribDivisor(i,d);}
};

}
}
