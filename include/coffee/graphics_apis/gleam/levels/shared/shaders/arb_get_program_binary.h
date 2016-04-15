#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_GetProgramBinary
{
    /* Programs */
    STATICINLINE void ProgramBinary(CGhnd h,CGenum t,uint64 size,c_cptr d)
    {glProgramBinary(h,t,d,size);}
    STATICINLINE void ProgramGetBinary(CGhnd h,int64 bsize,int32* size,CGenum* t,c_ptr d)
    {glGetProgramBinary(h,bsize,size,t,d);}
};

}
}
