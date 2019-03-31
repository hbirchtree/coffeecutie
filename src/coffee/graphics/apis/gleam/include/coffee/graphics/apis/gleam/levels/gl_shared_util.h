#pragma once

#include <coffee/image/cimage.h>
#include <coffee/core/CFiles>

#include "gl_shared_types.h"

namespace Coffee {
namespace CGL {

struct CGLUtil
{
    /*!
     * \brief Dump texture to file. Dirties texture binding.
     * \param t
     * \param h
     * \param fn
     */
    template<typename GL>
    static void DumpTexture(
            C_UNUSED(typename GL::Texture t),
            C_UNUSED(typename GL::CGhnd h),
            C_UNUSED(u32 l),
            C_UNUSED(cstring fn))
    {
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)

        Size tsize;
        GL::TexGetLevelParameteriv(t, l, GL_TEXTURE_WIDTH, &tsize.w);
        GL::TexGetLevelParameteriv(t, l, GL_TEXTURE_HEIGHT, &tsize.h);

        Bytes data = Bytes::Alloc(C_FCAST<szptr>(tsize.area() * 4));

        GL::TexBind(t, h);
        GL::TexGetImage(t, 0, PixCmp::RGBA, PixFmt::RGBA8, data);
        GL::TexBind(t, 0);

        Resource rsc(fn);

        auto outData = PNG::Save(stb::image_const::From(rsc, tsize));
        rsc          = outData;

        FileCommit(rsc);
#endif
    }
};

} // namespace CGL
} // namespace Coffee
