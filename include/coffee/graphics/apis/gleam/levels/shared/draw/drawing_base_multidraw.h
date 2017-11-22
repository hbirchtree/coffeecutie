#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_Drawing_Base_MultiDraw
{
    STATICINLINE void DrawMultiArrays(
            CGenum p,const i32* f,const i32* c, i32 dc)
    {glMultiDrawArrays(p,f,c,dc);}

    STATICINLINE void DrawMultiElements(
            CGenum p,const i32* f,TypeEnum t,const i64* off,i32 dc)
    {glMultiDrawElements(p,f,to_enum(t),(const void**)off,dc);}

    STATICINLINE void DrawMultiElementsBaseVertex(
            CGenum p,const i32* c,TypeEnum t,const i64* off,i32 dc,
            const i32* bv)
    {glMultiDrawElementsBaseVertex(p,c,to_enum(t),(const void**)off,
                                   dc,bv);}
};
}
}
