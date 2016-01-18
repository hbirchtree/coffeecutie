#include "cimage.h"

#include <base/cdebug.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace Coffee{
namespace CStbImageLib{

bool LoadData(CStbImage *target, const CResource *src)
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
    target->data = Alloc(size);
    CMemCpy(target->data,data,size);
}

bool Resize(CStbImage *img, const CSize &target, int channels)
{
    ubyte_t* data = (ubyte_t*)Alloc(img->bpp*img->size.h*img->size.w*channels);
    stbir_resize_uint8(img->data,img->size.w,img->size.h,0,
                       data,target.w,target.h,0,
                       channels);
    CFree(img->data);
    img->data = data;
    img->size = target;
    return img->data;
}

bool SavePNG(CResource *target, const CStbImageConst *src)
{
    return stbi_write_png_to_func(_stbi_write_data,target,
                                  src->size.w,src->size.h,
                                  src->bpp,src->data,src->size.w*4);
}

bool SavePNG(CResource *target, const CStbImage *src)
{
    return stbi_write_png_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data,src->size.w*4);
}

bool SaveTGA(CResource *target, const CStbImage *src)
{
    return stbi_write_tga_to_func(_stbi_write_data,target,src->size.w,src->size.h,src->bpp,src->data);
}

void FlipVertical(CStbImage *src)
{
    int32 wdt = src->size.w;
    szptr siz = src->bpp*src->size.w*src->size.h;

    ubyte_t* data = (ubyte_t*)Alloc(siz);

    for(szptr i=0;i<siz;i+=wdt*src->bpp)
    {
        CMemCpy(&data[i],&src->data[siz-wdt*src->bpp-i],wdt*src->bpp);
    }

    CFree(src->data);
    src->data = data;
}

void FlipHorizontal(CStbImage *src)
{
    int32 bot = src->size.h;
    int32 wdt = src->size.w;
    szptr siz = src->bpp*src->size.w*src->size.h;

    ubyte_t* data = (ubyte_t*)Alloc(siz);

    for(int32 i=0;i<bot;i++)
        for(int32 j=0;j<wdt;j++)
        {
            CMemCpy(&data[(i*wdt+wdt-j)*src->bpp],&src->data[(i*wdt+j)*src->bpp],src->bpp);
        }

    CFree(src->data);
    src->data = data;
}

void Error()
{
    cDebug("%s",stbi_failure_reason());
}

void ImageFree(CStbImage *img)
{
    CFree(img->data);
}

}

struct tga_header
{
    uint8 idsize;
    uint8 cmaptype;
    uint8 imgtype;
    uint16 cmapstart;
    uint16 cmapsize;
    uint8 cmapbpp;
    int16 xorg;
    int16 yorg;
    int16 width;
    int16 height;
    uint8 bpp;
    uint8 descriptor;
};

void CImage::SaveTGA(const CSize& resolution,
                      const CByteData& imgData,
                      CByteData& outdata)
{
    tga_header head;
    CMemClear(&head,sizeof(tga_header));

    head.width = resolution.w;
    head.height = resolution.h;

    head.bpp = 24;
    head.imgtype = 2;

    outdata.size = imgData.size+sizeof(tga_header);
    outdata.data = (byte_t*)Alloc(outdata.size);

    CMemCpy(&outdata.data[0],&head,sizeof(head));
    CMemCpy(&outdata.data[sizeof(tga_header)],imgData.data,imgData.size);
}

}
