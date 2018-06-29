#pragma once

#include "../tdef/integertypes.h"
#include "enumfun.h"

#undef None

namespace Coffee {

enum class PixelFlags : u16
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
C_FLAGS(PixelFlags, uint32);

enum class PixCmp : u8
{
    None,

    R,
    G,
    B,
    A,
    RG,
    RGB,
    RGBA,
    BGR,
    BGRA,

    Stencil,
    Depth,
    DepthStencil,
};

enum class PixelOperation : u8
{
    SwapEndiannes,
    Alignment,
};

enum class Filtering : u8
{
    None,
    Nearest,
    Linear,
};

enum class WrapPolicy : u8
{
    Repeat,
    MirrorRepeat,
    Clamp,
    MirrorClamp,
};

enum class PixelFormat : u8
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
    Depth16,          /* 16-bit unsigned short */
    Depth24Stencil8,  /* 32-bit data,           24-bit floating-point, 8-bit
                         integer */
    Depth32F,         /* 32-bit floating-point */
    Depth32FStencil8, /* 32-bit floating-point, 8-bit stencil */

    MAX_PIXFMT,

    Depth,
    DepthStencil, /* Fakes */

    /* Aliases */
    BCn  = S3TC,
    DXTn = S3TC,
    BPTC = S3TC,
    RGTC = S3TC,

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
enum class CompFlags : u8
{
    CompressionNone,

    ASTC_4x4,
    ASTC_5x4,
    ASTC_5x5,
    ASTC_6x5,
    ASTC_6x6,
    ASTC_8x5,
    ASTC_8x6,
    ASTC_8x8,
    ASTC_10x5,
    ASTC_10x6,
    ASTC_10x8,
    ASTC_10x10,
    ASTC_12x10,
    ASTC_12x12,

    S3TC_1, /* RGB */
    S3TC_3,
    S3TC_5, /* RGBA */

    BC1 = S3TC_1,
    BC3 = S3TC_5,
    BC4, /* R */
    BC5, /* RG */
    BC6H,
    BC7, /* RGBA */

    /* BPP specification, meant for PVRTC */
    bpp_2,
    bpp_4,

    Undefined = CompressionNone,
};
C_FLAGS(CompFlags, u8);

enum class BitFormat : u8
{
    Byte,
    ByteR,
    UByte,
    UByteR,
    UByte_332,
    UByte_233R,

    Short,
    ShortR,
    UShort,
    UShortR,
    UShort_4444,
    UShort_4444R,
    UShort_565,
    UShort_565R,
    UShort_5551,
    UShort_1555R,

    Int,
    IntR,
    UInt,
    UIntR,
    UInt_5999R,
    UInt_1010102,
    UInt_2101010R,

    Scalar_16,
    Scalar_32,
    Scalar_64,
    Scalar_11_11_10,

    UInt24_8,

    Scalar_32_Int_24_8,

    Undefined,
};

using BitFmt = BitFormat;
using PixFmt = PixelFormat;
using PixFlg = PixelFlags;

} // namespace Coffee

#include "pix_util.inl"
