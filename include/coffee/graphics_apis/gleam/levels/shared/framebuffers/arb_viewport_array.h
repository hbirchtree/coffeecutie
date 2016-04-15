#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_ViewportArray
{
    /* Use gl*Indexed gl*Arrayv */
    STATICINLINE void ViewportArrayv(uint32 i,size_t c,CRectF const* v)
    {glViewportArrayv(i,c,(scalar*)v);}
    STATICINLINE void ViewportSet(uint32 i,CRectF const* v){ViewportArrayv(i,1,v);}

    STATICINLINE void ScissorArrayv(uint32 i,size_t c,CRect const* v)
    {glScissorArrayv(i,c,(int32*)v);}
    STATICINLINE void ScissorSet(uint32 i,CRect const* v){ScissorArrayv(i,1,v);}

    STATICINLINE void DepthArrayv(uint32 i,size_t c,ZField64 const* v)
    {glDepthRangeArrayv(i,c,(bigscalar*)v);}
    STATICINLINE void DepthSet(uint32 i,ZField64 const* v){DepthArrayv(i,1,v);}
};

}
}
