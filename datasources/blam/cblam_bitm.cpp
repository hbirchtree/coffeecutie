#include "cblam_bitm.h"

#include "coffee/cdebug.h"

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
            out[y*w+x] = process(((uint32*)src)[y*w+x]);
        }

    return out;
}

}
}
