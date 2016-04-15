#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_BufferInvalidateSubData
{
    STATICINLINE void BufInvalidateData(CGhnd h)
    {glInvalidateBufferData(h);}
    STATICINLINE void BufInvalidateSubData(CGhnd h,int64 offset,uint32 size)
    {glInvalidateBufferSubData(h,offset,size);}
};
}
}
