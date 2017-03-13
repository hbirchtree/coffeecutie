#pragma once

#include "../gl_shared_types.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_Basic_Draw
{
    /* Arrays */
    STATICINLINE void DrawArrays(DrwMd const& p,int32 f,uint64 c)
    {glDrawArrays(to_enum(p),f,c);}
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void DrawArraysInstanced(DrwMd const& p,int32 f,uint32 c,uint32 ic)
    {glDrawArraysInstanced(to_enum(p),f,c,ic);}
#endif

    /* Elements */
    STATICINLINE void DrawElements(DrwMd const& p,uint32 c,TypeEnum t,uint64 off)
    {glDrawElements(to_enum(p),c,to_enum(t),(void*)off);}

#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void DrawElementsInstanced(DrwMd const& p,uint32 c,TypeEnum t,uint64 off,uint64 pc)
    {glDrawElementsInstanced(to_enum(p),c,to_enum(t),(void*)off,pc);}

    STATICINLINE void DrawRangeElements(DrwMd const& p,uint32 f,uint32 e,int32 c,TypeEnum t,int64 off)
    {glDrawRangeElements(to_enum(p),f,e,c,to_enum(t),(void*)off);}
#endif
};

}
}
