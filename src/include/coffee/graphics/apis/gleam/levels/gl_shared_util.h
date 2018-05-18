#pragma once

#include <coffee/CImage>
#include <coffee/core/CFiles>

#include "shared/gl_shared_types.h"

namespace Coffee{
namespace CGL{

struct CGLUtil
{
    /*!
     * \brief Dump texture to file. Dirties texture binding.
     * \param t
     * \param h
     * \param fn
     */
    template<typename GL>
    static void DumpTexture(typename GL::Texture t, typename GL::CGhnd h,
                            uint32 l, cstring fn)
    {
        C_USED(t);
        C_USED(h);
        C_USED(l);
        C_USED(fn);

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)

        CSize tsize;
        GL::TexGetLevelParameteriv(t,l,GL_TEXTURE_WIDTH,&tsize.w);
        GL::TexGetLevelParameteriv(t,l,GL_TEXTURE_HEIGHT,&tsize.h);

        Bytes data = Bytes::Alloc(C_FCAST<szptr>(tsize.area() * 4));

        GL::TexBind(t,h);
        GL::TexGetImage(t,0,PixelComponents::RGBA,PixelFormat::RGBA8,data);
        GL::TexBind(t,0);

        CResources::Resource rsc(fn);

        auto outData = PNG::Save(stb::image_const::From(rsc, tsize));
        rsc = outData;

        CResources::FileCommit(rsc);
        CFree(data.data);
#endif
    }
};

}
}
