#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_VertexAttribBinding
{
    /* VAO */
    STATICINLINE void VAOAttribFormat(uint32 i,uint32 s,TypeEnum t,bool n,uint32 off)
    {glVertexAttribFormat(i,s,to_enum(t),(n)?GL_TRUE:GL_FALSE,off);}
    STATICINLINE void VAOAttribFormatI(uint32 i,uint32 s,TypeEnum t,uint32 off)
    {glVertexAttribIFormat(i,s,to_enum(t),off);}

    STATICINLINE void VAOBindingDivisor(uint32 attr,uint32 div)
    {glVertexBindingDivisor(attr,div);}
    STATICINLINE void VAOAttribBinding(uint32 attr,uint32 idx)
    {glVertexAttribBinding(attr,idx);}
    STATICINLINE void VAOBindVertexBuffer(uint32 idx,CGhnd h,uint64 off,int32 stride)
    {glBindVertexBuffer(idx,h,off,stride);}

#ifdef COFFEE_GLEAM_DESKTOP
    STATICINLINE void VAOAttribFormatL(uint32 i,uint32 s,TypeEnum t,uint32 off)
    {glVertexAttribLFormat(i,s,to_enum(t),off);}
    STATICINLINE void VAOBindVertexBuffers(uint32 idx_f, uint32 c,const CGhnd* h,
                                           const GLintptr* off, const int32* stride)
    {glBindVertexBuffers(idx_f,c,h,off,stride);}
#endif
};
}
}
