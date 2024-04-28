#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

#undef None
#undef Undefined

namespace typing {
namespace pixels {

using namespace ::libc_types;

enum class pix_flags : u16
{
    None = 0x0,

    /* Misc. properties */
    FloatingPoint = 0x10,
    Unsigned      = 0x20,
    Normalize     = 0x40,
    Signed        = 0x80,
    Unormalized   = 0x100,

    /* Different color spaces */
    sRGB = 0x200,

    /* Specifying color channels for compressed formats */
    R    = 0x0800,
    RG   = 0x1000,
    RGB  = 0x2000,
    RGBA = 0x4000,

    /* Specializations */
    RGBA_Punchthrough = 0x8000,

    LOWER_SHORT_MASK = 0xFFFF,
};
C_FLAGS(pix_flags, u16);

enum class pix_fmt : u8
{
    None,
    /* Formatting: [CHANNELS][NUMBITS][DATATYPE] */

    /* Standard integer formats, float samplers */
    R8,   /*   8-bit */
    R16,  /*  16-bit */
    RG8,  /*   8-bit            8-bit R,  8-bit G */
    RG16, /*  32-bit,          16-bit R, 16-bit G */

    R3G3B2, /*   8-bit,          3-bit R, 3-bit G, 2-bit B */
    RGB4,   /*  12-bit,          4-bit per channel*/
    RGB5,   /*  15-bit,          5-bit per channel*/
    RGB565, /*  16-bit,          5-bit R, 6-bit G, 5-bit B */
    RGB8,   /*  24-bit,          8-bit per channel*/
    RGB10,  /*  30-bit,         10-bit per channel*/
    RGB12,  /*  36-bit,         12-bit per channel*/
    RGB16,  /*  48-bit,         16-bit per channel*/

    RGBA2,  /*   8-bit,          2-bit per channel */
    RGBA8,  /*  32-bit,          8-bit per channel*/
    RGBA4,  /*  16-bit,          4-bit per channel*/
    RGBA12, /*  48-bit,         12-bit per channel */
    RGBA16, /*  64-bit,         16-bit per channel*/

    RGB5A1,  /*  16-bit,          5-bit per channel, 1-bit alpha*/
    RGB9E5,  /*  32-bit,          9-bit special fp RGB, 5-bit ui extra */
    RGB10A2, /*  32-bit,         10-bit RGB, 2-bit alpha */

    /* I-sampled formats */
    R8I,     /*   8-bit i*/
    R16I,    /*  16-bit i */
    R32I,    /*  32-bit i */
    RG8I,    /*  16-bit,          8-bit i channels */
    RG16I,   /*  32-bit,         16-bit i channels */
    RG32I,   /*  64-bit,         32-bit i channels */
    RGB8I,   /*  24-bit,          8-bit i channels */
    RGB16I,  /*  48-bit,         16-bit i channels */
    RGB32I,  /*  96-bit,         32-bit i channels */
    RGBA8I,  /*  16-bit,          8-bit i channels */
    RGBA16I, /*  64-bit,         16-bit i channels */
    RGBA32I, /* 128-bit,         32-bit i channels */

    /* UI-sampled formats */
    R8UI,     /*   8-bit ui*/
    R16UI,    /*  16-bit ui */
    R32UI,    /*  32-bit ui */
    RG8UI,    /*  16-bit,          8-bit ui channels */
    RG16UI,   /*  32-bit,         16-bit ui channels */
    RG32UI,   /*  64-bit,         32-bit ui channels */
    RGB8UI,   /*  24-bit,          8-bit ui channels */
    RGB16UI,  /*  48-bit,         16-bit ui channels */
    RGB32UI,  /*  96-bit,         32-bit ui channels */
    RGBA8UI,  /*  16-bit,          8-bit ui channels */
    RGBA16UI, /*  64-bit,         16-bit ui channels */
    RGBA32UI, /* 128-bit,         32-bit ui channels */

    RGB10A2UI, /*  32-bit,         10-bit ui per channel, 2-bit ui alpha*/

    /* Floating-point formats */
    R16F,    /*  16-bit fp */
    R32F,    /*  32-bit fp */
    RG16F,   /*  32-bit,         16-bit fp channels */
    RG32F,   /*  64-bit,         32-bit fp channels */
    RGB16F,  /*  48-bit,         16-bit fp channels */
    RGB32F,  /*  96-bit,         32-bit fp channels */
    RGBA16F, /*  64-bit,         16-bit fp channels */
    RGBA32F, /* 128-bit,         32-bit fp channels */

    R11G11B10F, /*  32-bit,         11-bit fp RG, 10-bit fp B*/

    /* Compressed/nonlinear formats, use flags for further details */
    SRGB8,   /*  24-bit SRGB data*/
    SRGB8A8, /*  24-bit SRGB data + 8-bit integer*/

    ASTC,
    S3TC, /* Includes BPTC and RGTC */

    ATC,

    ETC1,
    ETC2,

    PVRTC,
    PVRTC2,

    /* Special data */
    //    Stencil,         /*  8-bit integer, not a real format */
    Depth16,         /* 16-bit unsigned short */
    Depth24,         /* 24-bit unsigned int */
    Depth24Stencil8, /* 32-bit data,           24-bit floating-point, 8-bit
                        integer */
    Depth32,
    Depth16F,
    Depth32F,         /* 32-bit floating-point */
    Depth32FStencil8, /* 32-bit floating-point, 8-bit stencil */

    MAX_PIXFMT,

    /* Fakes */
    Depth,
    DepthStencil,

    /* Aliases */
    BCn  = S3TC, /* Includes BC1-7, distinguished by CompFlags */
    DXTn = S3TC,
    BPTC = S3TC,
    RGTC = S3TC,

    EAC = ETC2,

    Undefined = None,
};

/*!
 * \brief Some compression formats are complicated
 *
 * For BC6/BC7/BPTC:
 * https://www.g-truc.net/post-0335.html
 * GL_ARB_texture_compression_bptc
 *
 */
enum class comp_flags : u16
{
    None,
    CompressionNone = None,

    ASTC_4x4   = 0x1,
    ASTC_5x4   = 0x2,
    ASTC_5x5   = 0x4,
    ASTC_6x5   = 0x8,
    ASTC_6x6   = 0x10,
    ASTC_8x5   = 0x20,
    ASTC_8x6   = 0x40,
    ASTC_8x8   = 0x80,
    ASTC_10x5  = 0x100,
    ASTC_10x6  = 0x200,
    ASTC_10x8  = 0x400,
    ASTC_10x10 = 0x800,
    ASTC_12x10 = 0x1000,
    ASTC_12x12 = 0x2000,

    BC1  = 0x1,  /* 8-bit RGB/RGBA, aka DXT1, 6bpp, 64 bits */
    BC2  = 0x2,  /* 8-bit RGBA, aka DXT3, 8bpp, 128 bits */
    BC3  = 0x4,  /* 8-bit RGBA, aka DXT5, 8bpp, 128 bits */
    BC4  = 0x8,  /* 8-bit R, aka RGTC, 4bpp, 64 bits */
    BC5  = 0x10, /* 8-bit RG, aka RGTC, 8bpp, 128 bits */
    BC6H = 0x20, /* RGBA half-precision float, aka BPTC, 128 bits */
    BC7  = 0x40, /* 8-bit RGBA, aka BPTC, 128 bits */

    /* BPP specification, meant for PVRTC */
    bpp_2 = 0x2,
    bpp_4 = 0x4,

    Undefined = CompressionNone,
};
C_FLAGS(comp_flags, u16);

enum class bit_fmt : u8
{
    i8,
    u8,
    u8_332,
    u8_233,

    i16,
    u16,
    u16_4444,
    u16_4444_r,
    u16_565,
    u16_565_r,
    u16_5551,
    u16_1555,

    i32,
    i32_1010102,

    u32,
    u32_5999,
    u32_1010102,
    u32_2101010,

    f16,
    f32,
    f64,
    Scalar_11_11_10,

    u32_24_8,

    f32_u32_24_8,

    undefined,
};

struct r11g11b10f
{
    u32 d;
};

static_assert(sizeof(r11g11b10f) == 4, "Invalid size");

PACKED(struct) r11g11b10u
{
#if defined(COFFEE_UNIXPLAT)
    u16 r:11;
    u16 g:11;
    u16 b:10;
#else
    u32 d;
#endif
};

static_assert(sizeof(r11g11b10u) == 4, "Invalid size");

struct r10g10b10a2
{
    u32 d;
};

static_assert(sizeof(r10g10b10a2) == 4, "Invalid size");

union r4g4b4a4
{
    u16 d;
};

static_assert(sizeof(r4g4b4a4) == 2, "Invalid size");

union r5g6b5
{
    u16 d;
};

static_assert(sizeof(r5g6b5) == 2, "Invalid size");

using f11 = r11g11b10f;
using u10 = r10g10b10a2;

} // namespace pixels
} // namespace typing
