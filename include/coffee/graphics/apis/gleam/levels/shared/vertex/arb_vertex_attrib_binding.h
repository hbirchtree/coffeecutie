#pragma once

#include "../gl_shared_enum_convert.h"
#include "../gl_shared_include.h"
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee {
namespace CGL {

template<typename ReqVer>
struct CGL_VertexAttribBinding
{
    /* VAO */
    GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
    STATICINLINE void VAOAttribFormat(uint32 i, i32 s, TypeEnum t, bool n,
                                      uint32 off)
    {
        glVertexAttribFormat(i, s, to_enum(t), (n) ? GL_TRUE : GL_FALSE, off);
    }
    STATICINLINE void VAOAttribFormatI(uint32 i, i32 s, TypeEnum t,
                                       uint32 off)
    {
        glVertexAttribIFormat(i, s, to_enum(t), off);
    }

    GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
    STATICINLINE void VAOBindingDivisor(uint32 attr, uint32 div)
    {
        glVertexBindingDivisor(attr, div);
    }
    STATICINLINE void VAOAttribBinding(uint32 attr, uint32 idx)
    {
        glVertexAttribBinding(attr, idx);
    }
    STATICINLINE void VAOBindVertexBuffer(uint32 idx, CGhnd h, ptroff off,
                                          int32 stride)
    {
        glBindVertexBuffer(idx, h, off, stride);
    }

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    STATICINLINE void VAOAttribFormatL(uint32 i, i32 s, TypeEnum t,
                                       uint32 off)
    {
        glVertexAttribLFormat(i, s, to_enum(t), off);
    }
    STATICINLINE void VAOBindVertexBuffers(uint32 idx_f, i32 c,
                                           const CGhnd* h, const GLintptr* off,
                                           const int32* stride)
    {
        glBindVertexBuffers(idx_f, c, h, off, stride);
    }
#endif
};
}
}
