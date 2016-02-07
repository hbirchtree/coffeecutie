#include <coffee/graphics_apis/opengl/glbinding/wrappers/ctexture_dxtc.h>

#include <coffee/graphics_apis/opengl/glfunctions.h>
#include <coffee/graphics_apis/opengl/glbinding/wrappers/ctexture.h>

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_dxtc_load(CTexture& texture, const CGint &blocksize, c_cptr data)
{
    CTextureSize res = texture.size;
    int32 i;
    szptr size;
    szptr offset = 0;
    coffee_graphics_bind(texture);
    for(i=0; i < texture.levels && (res.width || res.height || res.depth);i++)
    {
        if(res.width == 0)
            res.width = 1;
        if(res.height == 0)
            res.height = 1;
        if(res.depth == 0)
            res.depth = 1;

        size = ((res.width+3)/4)*((res.height+3)/4)*((res.depth+3)/4)*blocksize;

        switch(texture.size.dimensions())
        {
        case 1:
            glCompressedTexImage1D(
                        gl_get(texture.type),i,
                        gl_get(texture.format),
                        res.width,0,size,
                        ((ubyte_t*)data)+offset);
            break;
        case 2:
            glCompressedTexImage2D(
                        gl_get(texture.type),i,
                        gl_get(texture.format),
                        res.width,res.height,0,size,
                        ((ubyte_t*)data)+offset);
            break;
        case 3:
            glCompressedTexImage3D(
                        gl_get(texture.type),i,
                        gl_get(texture.format),
                        res.width,res.height,res.depth,
                        0,size,
                        ((ubyte_t*)data)+offset);
            break;
        }

        offset+=size;
        res.width >>= 1;
        res.height >>= 1;
        res.depth >>= 1;
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
                    tex.size.width,tex.size.height,tex.size.depth,
                    tex.size.width*tex.size.height*tex.size.depth*pixelSize,
                    0, data);
        break;
    case 2:
        glCompressedTexImage2D(
                    gl_get(tex.type),
                    level,gl_get(fmt),
                    tex.size.width,tex.size.height,
                    tex.size.width*tex.size.height*pixelSize,
                    0, data);
        break;
    case 1:
        glCompressedTexImage1D(
                    gl_get(tex.type),
                    level, gl_get(fmt),
                    tex.size.width, tex.size.width*pixelSize,
                    0, data);
        break;
    }
    coffee_graphics_unbind(tex);
}

}
}
