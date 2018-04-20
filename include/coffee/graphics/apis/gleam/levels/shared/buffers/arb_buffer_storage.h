#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

template<typename ReqVer>
struct CGL_BufferStorage
{
    GL_VERSION_REQ_DESKTOP(GLVER_44)
    STATICINLINE void BufStorage(BufType t,i64 s,c_cptr d,ResourceAccess a)
    {
        glBufferStorage(to_enum(t),s,d,to_enum2(a));
    }
};

}
}
