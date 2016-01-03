#include "glbinding/wrappers/ctexture_dxtc.h"

#include "glfunctions.h"
#include "glbinding/wrappers/ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_dxtc_load(CTexture& texture, const CGint &blocksize, c_cptr data)
{
    CTextureSize res = texture.size;
    int32 i;
    szptr size;
    szptr offset = 0;
    coffee_graphics_bind(texture);
    for(i=0; i < texture.levels && (res.w || res.h || res.d);i++)
    {
        if(res.w == 0)
            res.w = 1;
        if(res.h == 0)
            res.h = 1;
        if(res.d == 0)
            res.d = 1;

        size = ((res.w+3)/4)*((res.h+3)/4)*((res.d+3)/4)*blocksize;

        switch(texture.size.dimensions())
        {
        case 1:
            glCompressedTexImage1D(
                        gl_get(texture.type),i,
                        gl_get(texture.format),
                        res.w,0,size,
                        ((ubyte_t*)data)+offset);
            break;
        case 2:
            glCompressedTexImage2D(
                        gl_get(texture.type),i,
                        gl_get(texture.format),
                        res.w,res.h,0,size,
                        ((ubyte_t*)data)+offset);
            break;
        case 3:
            glCompressedTexImage3D(
                        gl_get(texture.type),i,
                        gl_get(texture.format),
                        res.w,res.h,res.d,
                        0,size,
                        ((ubyte_t*)data)+offset);
            break;
        }

        offset+=size;
        res.w >>= 1;
        res.h >>= 1;
        res.d >>= 1;
    }
    coffee_graphics_unbind(texture);
}

void coffee_graphics_tex_compressed_store(const CTexture &tex, CResources::CResource &res)
{
    glGetCompressedTextureImage(tex.handle,0,res.size,res.data);
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
