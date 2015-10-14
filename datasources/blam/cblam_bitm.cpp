#include "cblam_bitm.h"

#include "coffee/cdebug.h"
#include "coffee/cfiles.h"
#include "coffee_impl/image/cimage.h"
#include "coffee_impl/graphics/ctexture_dxtc.h"

namespace Coffee{
namespace CBlam{

blam_bitm_header _bitm_get_header(const void* base, int32 offset)
{
    const blam_bitm_header* ptr = (const blam_bitm_header*)(((const byte*)base)+offset);
    blam_bitm_header hdr;
    memcpy(&hdr,ptr,sizeof(blam_bitm_header));
    return hdr;
}

const blam_bitm_image *coffee_bitm_get(
        const blam_index_item *item,
        const blam_file_header *map,
        int32 magic,

        int32* numImages)
{
    blam_bitm_header hdr = _bitm_get_header(map,item->offset-magic);

    hdr.offset_first -= magic;
    hdr.imageOffset -= magic;

    const blam_bitm_image* img = (const blam_bitm_image*)(((const byte*)map)+hdr.imageOffset);

    return img;
}

uint32 *coffee_bitm_decode_micro(
        const blam_bitm_image *img, const void *map,
        BlamBitmProcess process)
{
    int16 w = img->isize.w;
    int16 h = img->isize.h;

    const ubyte* src = ((const ubyte*)map)+img->offset;
    uint32* out = (uint32*)calloc(w*h,sizeof(uint32));

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

uint32 *coffee_bitm_decode_image(const blam_bitm_image *img, const void *map)
{
    if(img->flags&blam_bitm_flag_linear)
//    {
        cDebug("Swizzle the shizzle!");
//        return nullptr;
//    }

    BlamBitmProcess process = nullptr;

    switch(img->format)
    {
    case blam_bitm_format_P8:
    case blam_bitm_format_Y8: process = coffee_bitm_decode_m_p8_y8; break;

    case blam_bitm_format_A8: process = coffee_bitm_decode_m_a8; break;
    case blam_bitm_format_AY8: process = coffee_bitm_decode_m_ay8; break;
    case blam_bitm_format_A8Y8: process = coffee_bitm_decode_m_a8y8; break;

    case blam_bitm_format_R5G6B5: process = coffee_bitm_decode_m_r5g6b5; break;
    case blam_bitm_format_A1R5G5B5: process = coffee_bitm_decode_m_a1r5g5b5; break;
    case blam_bitm_format_A4R4G4B4: process = coffee_bitm_decode_m_a4r4g4b4; break;
    case blam_bitm_format_A8R8G8B8: process = coffee_bitm_decode_m_a8r8g8b8; break;
    case blam_bitm_format_X8R8G8B8: process = coffee_bitm_decode_m_x8r8g8b8; break;

    case blam_bitm_format_DXT1:
    case blam_bitm_format_DXT2AND3:
    case blam_bitm_format_DXT4AND5:
    default: cDebug("Unsupported format: %i",img->format); return nullptr;
    }

    return coffee_bitm_decode_micro(img,map,process);
}

void coffee_bitm_dump(
        const blam_bitm_image *img,
        const void* bitmfile,
        cstring filename)
{
    CResources::CResource r(filename);

    uint32* data = coffee_bitm_decode_image(img,bitmfile);

    if(!data)
        return;

    CStbImageLib::CStbImageConst stb;
    stb.data = (ubyte*)data;
    stb.bpp = 4;
    stb.size.w = img->isize.w;
    stb.size.h = img->isize.h;

    CStbImageLib::coffee_stb_image_save_png(&r,&stb);
    r.save_data();
    r.free_data();

    free(data);
}

blam_bitm_texture_def coffee_bitm_get_texture(const blam_bitm_image *img, const void *bitmfile)
{
    CASSERT(img->format <= blam_bitm_format_P8);

    blam_bitm_texture_def def;

    def.mipmaps = CMath::max<int16>(1,img->mipmaps);
    def.type = img->type;
    def.resolution = img->isize;

    switch(img->format)
    {
    case CBlam::blam_bitm_format_DXT1:
        def.format = blam_bitm_tex_DXT1;
        break;
    case CBlam::blam_bitm_format_DXT2AND3:
        def.format = blam_bitm_tex_DXT3;
        break;
    case CBlam::blam_bitm_format_DXT4AND5:
        def.format = blam_bitm_tex_DXT5;
        break;
    default:
        def.format = blam_bitm_tex_RGBA;
        break;
    };

    switch(img->format)
    {
    case blam_bitm_format_DXT1:
    case blam_bitm_format_DXT2AND3:
    case blam_bitm_format_DXT4AND5:{
        def.data = ((ubyte*)bitmfile)+img->offset;
        break;
    }
    default:
        def.data = coffee_bitm_decode_image(img,bitmfile);
        break;
    }

    return def;
}

}
}
