#pragma once

#include <coffee/CImage>
#include <coffee/core/CFiles>

#include "shared/gl_shared_include.h"

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
#ifdef COFFEE_GLEAM_DESKTOP

        CSize tsize;
        GL::TexGetLevelParameteriv(t,l,GL_TEXTURE_WIDTH,&tsize.w);
        GL::TexGetLevelParameteriv(t,l,GL_TEXTURE_HEIGHT,&tsize.h);

        Bytes data = {};

        data.size = tsize.area()*4;
        data.data = (ubyte_t*)Alloc(data.size);

        GL::TexBind(t,h);
        GL::TexGetImage(t,0,PixelComponents::RGBA,PixelFormat::RGBA8,data);
        GL::TexBind(t,0);

        CResources::Resource rsc(fn);

        CStbImageLib::CStbImageConst img;
        img.bpp = 4;
        img.data = data.data;
        img.size = tsize;

        CStbImageLib::SavePNG(&rsc,&img);

        CResources::FileCommit(rsc);
        CResources::FileFree(rsc);
        CFree(data.data);
#endif
    }
};

}
}
