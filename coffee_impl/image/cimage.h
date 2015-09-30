#ifndef CIMAGE_LIB
#define CIMAGE_LIB

#include "coffee_types.h"
#include "coffee/cfiles.h"
#include "coffee/cmath.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace Coffee{
namespace CStbImageLib{

using namespace CResources;

struct CStbImage
{
    ubyte* data = nullptr;
    CSize size;
    int bpp = 0;
};

static bool coffee_stb_image_load(CStbImage* target, CResource* src)
{
    target->data = stbi_load_from_memory(
                (const ubyte*)src->data,src->size,
                &target->size.w,&target->size.h,
                &target->bpp,0);
    return true;
}

static void _stbi_write_data(void* ctxt, void* data, int size)
{
    CResource* target = (CResource*)ctxt;
    target->size = size;
    target->data = malloc(size);
    memcpy(target->data,data,size);
}

static bool coffee_stb_image_resize(CStbImage* img, const CSize& target,int channels)
{
    ubyte* data = (ubyte*)malloc(img->bpp*img->size.h*img->size.w*channels);
    stbir_resize_uint8(img->data,img->size.w,img->size.h,0,
                       data,target.w,target.h,0,
                       channels);
    free(img->data);
    img->data = data;
    img->size = target;
}

static bool coffee_stb_image_save_png(CResource* target, CStbImage* src)
{
    return stbi_write_png_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data,src->size.w*4);
}

static bool coffee_stb_image_save_tga(CResource* target, CStbImage* src)
{
    return stbi_write_tga_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data);
}

} //CStbImageLib
} //Coffee

#endif
