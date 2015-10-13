#include "ctexture_dxtc.h"

#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

CTexture *coffee_graphics_tex_dxtc_load(const CDXTCHeader *rsc)
{
    CTexture* tex = new CTexture;
    coffee_graphics_alloc(tex);

    tex->textureType = GL_TEXTURE_2D;
    tex->levels = rsc->mipmaps;

//    CTextureTools::CTextureData dat;
//    dat.format = GL_RGBA8;
//    CTextureTools::coffee_create_texturesize(&dat,rsc->resolution.w,rsc->resolution.h);

//    CTextureTools::coffee_graphics_tex_2d_define_safe(tex,&dat);

    CSize res = rsc->resolution;
    csize_t i;
    szptr size;
    szptr offset = 0;
    coffee_graphics_bind(tex);

    for(i=0; i < rsc->mipmaps && (res.w || res.h);i++)
    {
        if(res.w == 0)
            res.w = 1;
        if(res.h == 0)
            res.h = 1;

        size = ((res.w+3)/4)*((res.h+3)/4)*rsc->blockSize;

        glCompressedTexImage2D(tex->textureType,i,rsc->internalFormat,
                               res.w,res.h,0,size,((ubyte*)rsc->data)+offset);

        offset+=size;
        res.w >>= 1;
        res.h >>= 1;
    }
    coffee_graphics_unbind(tex);

    coffee_graphics_tex_param(tex,GL_TEXTURE_BASE_LEVEL,0);
    coffee_graphics_tex_param(tex,GL_TEXTURE_MAX_LEVEL,rsc->mipmaps-1);

    return tex;
}

}
}
