#include "cimage.h"

#include <coffee/core/graphics/opengl_glbinding/glbinding.h>

#include <coffee/core/base/cdebug.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace Coffee{
namespace CStbImageLib{

bool coffee_stb_image_load(CStbImage *target, const CResource *src)
{
    target->data = stbi_load_from_memory(
                (const ubyte_t*)src->data,src->size,
                &target->size.w,&target->size.h,
                &target->bpp,STBI_rgb_alpha);
    return true;
}

void _stbi_write_data(void *ctxt, void *data, int size)
{
    CResource* target = (CResource*)ctxt;
    target->size = size;
    target->data = c_alloc(size);
    c_memcpy(target->data,data,size);
}

bool coffee_stb_image_resize(CStbImage *img, const CSize &target, int channels)
{
    ubyte_t* data = (ubyte_t*)c_alloc(img->bpp*img->size.h*img->size.w*channels);
    stbir_resize_uint8(img->data,img->size.w,img->size.h,0,
                       data,target.w,target.h,0,
                       channels);
    c_free(img->data);
    img->data = data;
    img->size = target;
    return img->data;
}

bool coffee_stb_image_save_png(CResource *target, const CStbImageConst *src)
{
    return stbi_write_png_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data,src->size.w*4);
}

bool coffee_stb_image_save_png(CResource *target, const CStbImage *src)
{
    return stbi_write_png_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data,src->size.w*4);
}

bool coffee_stb_image_save_tga(CResource *target, const CStbImage *src)
{
    return stbi_write_tga_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data);
}

void coffee_stb_image_flip_vertical(CStbImage *src)
{
    int32 wdt = src->size.w;
    szptr siz = src->bpp*src->size.w*src->size.h;

    ubyte_t* data = (ubyte_t*)c_alloc(siz);

    for(szptr i=0;i<siz;i+=wdt*src->bpp)
    {
        c_memcpy(&data[i],&src->data[siz-wdt*src->bpp-i],wdt*src->bpp);
    }

    c_free(src->data);
    src->data = data;
}

void coffee_stb_image_flip_horizontal(CStbImage *src)
{
    int32 bot = src->size.h;
    int32 wdt = src->size.w;
    szptr siz = src->bpp*src->size.w*src->size.h;

    ubyte_t* data = (ubyte_t*)c_alloc(siz);

    for(int32 i=0;i<bot;i++)
        for(int32 j=0;j<wdt;j++)
        {
            c_memcpy(&data[(i*wdt+wdt-j)*src->bpp],&src->data[(i*wdt+j)*src->bpp],src->bpp);
        }

    c_free(src->data);
    src->data = data;
}

void coffee_stb_error()
{
    cDebug("%s",stbi_failure_reason());
}

void coffee_stb_image_free(CStbImage *img)
{
    c_free(img->data);
}


void coffee_graphics_tex_dump(
        const CTexture *tex, cstring filename)
{
    CStbImageLib::CStbImage img;

    img.bpp = 4;
    img.size.w = tex->size.w;
    img.size.h = tex->size.h;

    coffee_graphics_tex_download_texture(
                tex,0,img.bpp*img.size.w*img.size.h,
                CTexFormat::RGBA,&img);


    CResources::CResource fl(filename);
    CStbImageLib::coffee_stb_image_save_png(&fl,&img);
    coffee_file_commit(&fl);
    coffee_file_free(&fl);

    c_free(img.data);
}


}

namespace CGraphicsWrappers{

void coffee_graphics_tex_download_texture(const CTexture *tex, CGint level,
        CGsize size, CTexFormat format,
        CStbImageLib::CStbImage *img)
{
    img->data = (ubyte_t*)c_alloc(size);
    glGetTextureImage(
                tex->handle,
                level,
                gl_get(format),
                GL_UNSIGNED_BYTE,
                size,img->data);
}

CTextureData *coffee_graphics_tex_create_texdata(
        CResources::CResource const& resource, c_ptr location)
{
    CStbImageLib::CStbImage img;
    if(!coffee_stb_image_load(&img,&resource))
        return nullptr;

    CTextureData* ptr;
    if(location)
        new (location) CTextureData;
    else
        ptr = new CTextureData;

    ptr->data = img.data;
    ptr->size.w = img.size.w;
    ptr->size.h = img.size.h;

    switch(img.bpp)
    {
    case 4:
        ptr->format = CTexFormat::RGBA;
        break;
    case 3:
        ptr->format = CTexFormat::RGB;
        break;
    case 2:
        ptr->format = CTexFormat::RG;
        break;
    case 1:
        ptr->format = CTexFormat::RED;
        break;
    default:
        ptr->format = CTexFormat::RGBA;
    }

    ptr->datatype = CDataType::UByte;

    return ptr;
}

void coffee_graphics_tex_free_texdata(CTextureData *texd)
{
    free(texd->data);
    delete texd;
}

CImportedTexture coffee_graphics_tex_create_rtexdata(const CResources::CResource &resource)
{
    return CImportedTexture(coffee_graphics_tex_create_texdata(resource,nullptr));
}

}

}
