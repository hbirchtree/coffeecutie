#pragma once

#include "../gl_shared_enum_convert.h"
#include "../gl_shared_include.h"
#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee {
namespace CGL {

template <typename ReqVer>
struct CGL_BufferStorage {
    GL_VERSION_REQ_DESKTOP(GLVER_44)
    STATICINLINE void BufStorage(BufType t, Bytes const& data, RSCA a)
    {
        glBufferStorage(to_enum(t), C_FCAST<ptroff>(data.size), data.data,
                        to_enum2(a));
    }
};
}
}
