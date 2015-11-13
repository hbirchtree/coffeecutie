#include "cimage.h"

#include <coffee/core/base/cdebug.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace Coffee{
namespace CStbImageLib{

bool coffee_stb_image_load(CStbImage *target, CResource *src)
{
    target->data = stbi_load_from_memory(
                (const ubyte*)src->data,src->size,
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
    ubyte* data = (ubyte*)c_alloc(img->bpp*img->size.h*img->size.w*channels);
    stbir_resize_uint8(img->data,img->size.w,img->size.h,0,
                       data,target.w,target.h,0,
                       channels);
    c_free(img->data);
    img->data = data;
    img->size = target;
    return img->data;
}

bool coffee_stb_image_save_png(CResource *target, CStbImageConst *src)
{
    return stbi_write_png_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data,src->size.w*4);
}

bool coffee_stb_image_save_png(CResource *target, CStbImage *src)
{
    return stbi_write_png_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data,src->size.w*4);
}

bool coffee_stb_image_save_tga(CResource *target, CStbImage *src)
{
    return stbi_write_tga_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data);
}

void coffee_stb_image_flip_vertical(CStbImage *src)
{
    int32 wdt = src->size.w;
    szptr siz = src->bpp*src->size.w*src->size.h;

    ubyte* data = (ubyte*)c_alloc(siz);

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

    ubyte* data = (ubyte*)c_alloc(siz);

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

}
}
