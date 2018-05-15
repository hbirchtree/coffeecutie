#pragma once

#include "../gl_shared_types.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_Basic_Draw
{
    /* Arrays */
    STATICINLINE void DrawArrays(
            DrwMd const& p,i32 f,u64 c)
    {glDrawArrays(to_enum(p),f,c);}

#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE void DrawArraysInstanced(
            DrwMd const& p,i32 f, u32 c,u32 ic)
    {glDrawArraysInstanced(to_enum(p),f,c,ic);}
#endif

    /* Elements */
    STATICINLINE void DrawElements(
            DrwMd const& p,u32 c,TypeEnum t,u64 off)
    {glDrawElements(to_enum(p),c,to_enum(t),(void*)off);}

#if GL_VERSION_VERIFY(0x330, 0x300)
#if GL_VERSION_VERIFY(0x330, 0x320)
    STATICINLINE void DrawElementsBaseVertex(
            DrwMd const& p, u32 c, TypeEnum t, u64 off, i32 voff)
    {glDrawElementsBaseVertex(to_enum(p), c, to_enum(t), (void*)off, voff);}
    STATICINLINE void DrawElementInstancedBaseVertex(
            DrwMd const& p,u32 c,TypeEnum t,u64 off,u64 pc,i32 voff)
    {glDrawElementsInstancedBaseVertex(
                    to_enum(p), c, to_enum(t), (void*)off, pc, voff);}
#endif

    STATICINLINE void DrawElementsInstanced(
            DrwMd const& p,u32 c,TypeEnum t,uint64 off,u64 pc)
    {glDrawElementsInstanced(to_enum(p),c,to_enum(t),(void*)off,pc);}


    STATICINLINE void DrawRangeElements(
            DrwMd const& p,u32 f,u32 e,i32 c,TypeEnum t,i64 off)
    {glDrawRangeElements(to_enum(p),f,e,c,to_enum(t),(void*)off);}
#endif
};

}
}
