#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_Drawing_33
{
    STATICINLINE void DrawElementsBaseVertex(
            CGenum p,int32 c,TypeEnum t,int64 off,int32 bv)
    {glDrawElementsBaseVertex(p,c,to_enum(t),(void*)off,bv);}

    STATICINLINE void DrawElementsInstancedBaseVertex(
            CGenum p,int32 c,TypeEnum t,int64 off,int32 pc,int32 bv)
    {glDrawElementsInstancedBaseVertex(p,c,to_enum(t),(void*)off,pc,bv);}

    STATICINLINE void DrawRangeElementsBaseVertex(
            CGenum p,uint32 f,uint32 e,int32 c,TypeEnum t,int64 off,int32 bv)
    {glDrawRangeElementsBaseVertex(p,f,e,c,to_enum(t),(void*)off,bv);}
};
}
}
