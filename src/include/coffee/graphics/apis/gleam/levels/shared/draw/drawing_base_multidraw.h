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
            DrwMd const& d,const i32* f,const i32* c, i32 dc)
    {glMultiDrawArrays(
                    to_enum(d.t,d.c),f,c,dc);}

    STATICINLINE void DrawMultiElements(
            DrwMd const& d,const i32* f,TypeEnum t,const i64* off,i32 dc)
    {glMultiDrawElements(
                    to_enum(d.t,d.c),f,to_enum(t),(const void**)off,dc);}

    STATICINLINE void DrawMultiElementsBaseVertex(
            DrwMd const& d,const i32* c,TypeEnum t,const i64* off,i32 dc,
            const i32* bv)
    {glMultiDrawElementsBaseVertex(
                    to_enum(d.t,d.c),c,to_enum(t),(const void**)off,
                    dc,bv);}
};
}
}
