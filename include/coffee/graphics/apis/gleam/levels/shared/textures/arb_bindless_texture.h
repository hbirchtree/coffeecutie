#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{
namespace CGL{

GL_EXTENSION_REQ("GL_ARB_bindless_texture")
struct CGL_BindlessTexture
{
    /* Uniforms */
    STATICINLINE void UniformHandlei64v(CGhnd hnd, i32 location,
                                        Span<CGhnd64> const& handles)
    {
        glProgramUniformHandleui64vARB(hnd, location,
                                       C_FCAST<i32>(handles.size),
                                       handles.data);
    }

    /* Texture handles/bindless handles */
    STATICINLINE CGhnd64 TexGetImageHandle(CGhnd hnd, i32 level, bool layered,
                                           i32 layer, PixFmt fmt)
    {
        return glGetImageHandleARB(hnd, level, layered, layer, to_enum(fmt));
    }

    STATICINLINE void TexMakeHandleResident(CGhnd64 h)
    {
        glMakeTextureHandleResidentARB(h);
    }

    STATICINLINE void TexMakeHandleNonResident(CGhnd64 h)
    {
        glMakeTextureHandleNonResidentARB(h);
    }

    STATICINLINE void ImgMakeHandleResident(CGhnd64 h, PixFmt fmt)
    {
        glMakeImageHandleResidentARB(h, to_enum(fmt));
    }
    STATICINLINE void ImgMakeHandleNonResident(CGhnd64 h)
    {
        glMakeImageHandleNonResidentARB(h);
    }

    STATICINLINE bool IsTexHandleResident(CGhnd64 h)
    {
        return glIsTextureHandleResidentARB(h);
    }
    STATICINLINE bool IsImgHandleResident(CGhnd64 h)
    {
        return glIsImageHandleResidentARB(h);
    }

    /* Samplers */
    STATICINLINE CGhnd64 TexGetSamplerHandle(CGhnd tex, CGhnd sampler)
    {
        return glGetTextureSamplerHandleARB(tex, sampler);
    }
};

}
}
