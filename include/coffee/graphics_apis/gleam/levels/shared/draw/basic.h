#pragma once

#include "../gl_shared_types.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_Basic_Draw
{
    /* Arrays */
    STATICINLINE void DrawArrays(CGenum p,int32 f,int64 c)
    {glDrawArrays(p,f,c);}
    STATICINLINE void DrawArraysInstanced(CGenum p,int32 f,int32 c,int32 ic)
    {glDrawArraysInstanced(p,f,c,ic);}

    /* Elements */
    STATICINLINE void DrawElements(CGenum p,int32 c,CGenum t,int64 off)
    {glDrawElements(p,c,t,(void*)off);}
    STATICINLINE void DrawElementsInstanced(CGenum p,int32 c,TypeEnum t,int64 off,uint64 pc)
    {glDrawElementsInstanced(p,c,to_enum(t),(void*)off,pc);}

    STATICINLINE void DrawRangeElements(CGenum p,uint32 f,uint32 e,int32 c,TypeEnum t,int64 off)
    {glDrawRangeElements(p,f,e,c,to_enum(t),(void*)off);}
};

}
}
