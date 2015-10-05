#ifndef CBLAM_BITM
#define CBLAM_BITM

#include "cblam_map.h"
#include "coffee_types.h"

namespace Coffee{
namespace CBlam{

enum blam_bitm_formats
{
    blam_bitm_format_A8         = 0x00,
    blam_bitm_format_Y8         = 0x01,
    blam_bitm_format_AY8        = 0x02,
    blam_bitm_format_A8Y8       = 0x03,
    blam_bitm_format_R5G6B5     = 0x06,
    blam_bitm_format_A1R5G5B5   = 0x08,
    blam_bitm_format_A4R4G4B4   = 0x09,
    blam_bitm_format_X8R8G8B8   = 0x0A,
    blam_bitm_format_A8R8G8B8   = 0x0B,
    blam_bitm_format_DXT1       = 0x0E,
    blam_bitm_format_DXT2AND3   = 0x0F,
    blam_bitm_format_DXT4AND5   = 0x10,
    blam_bitm_format_P8         = 0x11,
};

enum blam_bitm_types
{
    blam_bitm_type_2D   = 0x0,
    blam_bitm_type_3D   = 0x1,
    blam_bitm_type_cube = 0x2,
};

enum blam_bitm_flags
{
    blam_bitm_flag_linear = 0x10,
};

struct blam_rgba
{
    uint32 r,g,b,a;
};

struct blam_bitm_header
{
    int32 unknown[22];
    int32 offset_first;
    int32 unknown23;
    int32 imageCount;
    int32 imageOffset;
    int32 unknown25;
};

struct blam_bitm_padding
{
    int32 unknown[16];
};

struct blam_bitm_image
{
    int32   id;
    _cbasic_size<int16> isize;
    int16   depth;
    int16   type;
    int16   format;
    int16   flags;
    _cbasic_point<int16> reg_pnt;
    int16   mipmaps;
    int16   pixOffset;
    int32   offset;
    int32   size;
    int32   unknown[4];
};

static uint32 blam_rgba_to_int(const blam_rgba& c)
{
    return (c.r << 24) | (c.g << 16) | (c.b << 8) | c.a;
}

//To get the bitm_image from an item
extern const blam_bitm_image* coffee_bitm_get(
        const blam_index_item* item,
        const blam_file_header* map,
        int32 magic,
        int32 *numImages);

extern uint32 *coffee_bitm_decode_a8r8g8b8(
        const blam_bitm_image* img,
        const blam_file_header* map);

}
}

#endif
