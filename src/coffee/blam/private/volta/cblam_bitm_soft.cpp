#include <coffee/blam/volta/cblam_bitm_soft.h>

namespace Coffee{
namespace CBlam{

uint32 *coffee_bitm_soft_decode_image(const bitm_image_t *img, const void *map)
{
    if(img->flags&linear)
//    {
        cDebug("Swizzle the shizzle!");
//        return nullptr;
//    }

    BlamBitmProcess process = nullptr;

    switch(img->format)
    {
    case P8:
    case Y8: process = coffee_bitm_decode_m_p8_y8; break;

    case A8: process = coffee_bitm_decode_m_a8; break;
    case AY8: process = coffee_bitm_decode_m_ay8; break;
    case A8Y8: process = coffee_bitm_decode_m_a8y8; break;

    case R5G6B5: process = coffee_bitm_decode_m_r5g6b5; break;
    case A1R5G5B5: process = coffee_bitm_decode_m_a1r5g5b5; break;
    case A4R4G4B4: process = coffee_bitm_decode_m_a4r4g4b4; break;
    case A8R8G8B8: process = coffee_bitm_decode_m_a8r8g8b8; break;
    case X8R8G8B8: process = coffee_bitm_decode_m_x8r8g8b8; break;

    case DXT1:
    case DXT2AND3:
    case DXT4AND5:
    default: cWarning("DXT* formats are not supported in software mode!"); return nullptr;
    }

    return coffee_bitm_soft_decode_micro(img,map,process);
}

uint32 *coffee_bitm_soft_decode_micro(
        const bitm_image_t *img, const void *map,
        BlamBitmProcess process)
{
    int16 w = img->isize.w;
    int16 h = img->isize.h;

    const ubyte_t* src = ((const ubyte_t*)map)+img->offset;
    uint32* out = (uint32*)c_calloc(w*h,sizeof(uint32));

    for(int16 x=0;x<w;x++)
        for(int16 y=0;y<h;y++)
        {
            out[y*w+x] = process(
                        ((uint32*)src)[y*w+x],
                    ((uint16*)src)[y*w+x],
                    src[y*w+x]);
        }

    return out;
}

void coffee_bitm_dump(
        const bitm_image_t *img,
        const void* bitmfile,
        cstring filename)
{
    CResources::CResource r(filename);

    uint32* data = coffee_bitm_decode_image(img,bitmfile);

    if(!data)
        return;

    CStbImageLib::CStbImageConst stb;
    stb.data = (ubyte_t*)data;
    stb.bpp = 4;
    stb.size.w = img->isize.w;
    stb.size.h = img->isize.h;

    CStbImageLib::coffee_stb_image_save_png(&r,&stb);
    coffee_file_commit(r);
    coffee_file_free(r);

    c_free(data);
}

}
}
