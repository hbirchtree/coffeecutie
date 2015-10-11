#include "cblam_bitm.h"

namespace Coffee{
namespace CBlam{

struct blam_rgba
{
    uint32 r,g,b,a;
};

uint32 blam_rgba_to_int(const blam_rgba &c);

blam_bitm_header _bitm_get_header(const void* base, int32 offset)
{
    const blam_bitm_header* ptr = (const blam_bitm_header*)(((const byte*)base)+offset);
    blam_bitm_header hdr;
    memcpy(&hdr,ptr,sizeof(blam_bitm_header));
    return hdr;
}

uint32 blam_rgba_to_int(const blam_rgba &c)
{
    return (c.r << 24) | (c.g << 16) | (c.b << 8) | c.a;
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

uint32 *coffee_bitm_decode_a8r8g8b8(const blam_bitm_image *img, const void *map)
{
    int32 w = img->isize.w;
    int32 h = img->isize.h;

    uint32* out = (uint32*)calloc(w*h,sizeof(uint32));

    blam_rgba col;
    uint32 cdata;

    for(int16 x=0;x<w;x++)
        for(int16 y=0;y<h;y++)
        {
            cdata = ((uint32*)(((const ubyte*)map)+img->offset))[y*w+x];

            col.a = (cdata >> 24);
            col.r = (cdata >> 16) & 0xff;
            col.g = (cdata >> 8) & 0xff;
            col.b = (cdata) & 0xff;

            out[y*w+x] = blam_rgba_to_int(col);
        }

    return out;
}


}
}
