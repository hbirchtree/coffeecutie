#include "wrappers/ctexture_dxtc.h"

#include "glfunctions.h"
#include "wrappers/ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

CTexture *coffee_graphics_tex_dxtc_load(const CBlamDXTCHeader &rsc, c_ptr constructLocation)
{
    CTexture* tex;
    if(!constructLocation)
        tex = new CTexture;
    else
        tex = new (constructLocation) CTexture;
    coffee_graphics_alloc(*tex);

    tex->type = CTexType::Tex2D;
    tex->levels = rsc.mipmaps;

    CTextureSize res = rsc.resolution;
    szptr i;
    szptr size;
    szptr offset = 0;
    coffee_graphics_bind(*tex);
    for(i=0; i < rsc.mipmaps && (res.w || res.h);i++)
    {
        if(res.w == 0)
            res.w = 1;
        if(res.h == 0)
            res.h = 1;

        size = ((res.w+3)/4)*((res.h+3)/4)*rsc.blockSize;

        glCompressedTexImage2D(
                    gl_get(tex->type),
                    i,
                    gl_get(rsc.internalFormat),
                    res.w,res.h,0,size,
                    ((ubyte_t*)rsc.data)+offset);

        offset+=size;
        res.w >>= 1;
        res.h >>= 1;
    }
    coffee_graphics_unbind(*tex);

    return tex;
}

void coffee_graphics_tex_compressed_store(const CTexture &tex, CResources::CResource &res)
{
    glGetCompressedTextureImage(tex.handle,0,res.size,res.data);
}

CBlamDXTCHeader::CBlamDXTCHeader():
    data(nullptr),
    mipmaps(0),
    blockSize(0),
    internalFormat(CTexIntFormat::DXT1)
{

}

void coffee_graphics_tex_compressed_load(
        CTexture &tex, const CGint &level,
        const CTexIntFormat &fmt, const CGint& pixelSize,
        c_cptr data)
{
    coffee_graphics_bind(tex);
    switch(tex.size.dimensions())
    {
    case 3:
        glCompressedTexImage3D(
                    gl_get(tex.type),
                    level,gl_get(fmt),
                    tex.size.w,tex.size.h,tex.size.d,
                    tex.size.w*tex.size.h*tex.size.d*pixelSize,
                    0, data);
        break;
    case 2:
        glCompressedTexImage2D(
                    gl_get(tex.type),
                    level,gl_get(fmt),
                    tex.size.w,tex.size.h,
                    tex.size.w*tex.size.h*pixelSize,
                    0, data);
        break;
    case 1:
        glCompressedTexImage1D(
                    gl_get(tex.type),
                    level, gl_get(fmt),
                    tex.size.w, tex.size.w*pixelSize,
                    0, data);
        break;
    }
    coffee_graphics_unbind(tex);
}

}
}
