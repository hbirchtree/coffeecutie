#pragma once

#include "enumfun.h"
#include "../tdef/integertypes.h"

#undef None

namespace Coffee{

enum class PixelFlags : u16
{
    None = 0x0,
    /* Misc. properties */
    FloatingPoint   = 0x10,
    Unsigned        = 0x20,
    Normalize       = 0x40,
    Signed          = 0x80,
    Unormalized     = 0x100,

    /* Specifying color channels for compressed formats */
    R       = 0x0800,
    RG      = 0x1000,
    RGB     = 0x2000,
    RGBA    = 0x4000,
    SRGBA   = 0x8000,
};
C_FLAGS(PixelFlags,uint32);

enum class PixelComponents : u8
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

using PixCmp = PixelComponents;

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
    R8,         /*   8-bit */
    R16,        /*  16-bit */
    RG8,        /*   8-bit            8-bit R,  8-bit G */
    RG16,       /*  32-bit,          16-bit R, 16-bit G */

    R3G3B2,     /*   8-bit,          3-bit R, 3-bit G, 2-bit B */
    RGB4,       /*  12-bit,          4-bit per channel*/
    RGB5,       /*  15-bit,          5-bit per channel*/
    RGB565,     /*  16-bit,          5-bit R, 6-bit G, 5-bit B */
    RGB8,       /*  24-bit,          8-bit per channel*/
    RGB10,      /*  30-bit,         10-bit per channel*/
    RGB12,      /*  36-bit,         12-bit per channel*/
    RGB16,      /*  48-bit,         16-bit per channel*/

    RGBA2,      /*   8-bit,          2-bit per channel */
    RGBA8,      /*  32-bit,          8-bit per channel*/
    RGBA4,      /*  16-bit,          4-bit per channel*/
    RGBA12,     /*  48-bit,         12-bit per channel */
    RGBA16,     /*  64-bit,         16-bit per channel*/

    RGB5A1,     /*  16-bit,          5-bit per channel, 1-bit alpha*/
    RGB9E5,     /*  32-bit,          9-bit special fp RGB, 5-bit ui extra */
    RGB10A2,    /*  32-bit,         10-bit RGB, 2-bit alpha */

    /* I-sampled formats */
    R8I,        /*   8-bit i*/
    R16I,       /*  16-bit i */
    R32I,       /*  32-bit i */
    RG8I,       /*  16-bit,          8-bit i channels */
    RG16I,      /*  32-bit,         16-bit i channels */
    RG32I,      /*  64-bit,         32-bit i channels */
    RGB8I,      /*  24-bit,          8-bit i channels */
    RGB16I,     /*  48-bit,         16-bit i channels */
    RGB32I,     /*  96-bit,         32-bit i channels */
    RGBA8I,     /*  16-bit,          8-bit i channels */
    RGBA16I,    /*  64-bit,         16-bit i channels */
    RGBA32I,    /* 128-bit,         32-bit i channels */

    /* UI-sampled formats */
    R8UI,       /*   8-bit ui*/
    R16UI,      /*  16-bit ui */
    R32UI,      /*  32-bit ui */
    RG8UI,      /*  16-bit,          8-bit ui channels */
    RG16UI,     /*  32-bit,         16-bit ui channels */
    RG32UI,     /*  64-bit,         32-bit ui channels */
    RGB8UI,     /*  24-bit,          8-bit ui channels */
    RGB16UI,    /*  48-bit,         16-bit ui channels */
    RGB32UI,    /*  96-bit,         32-bit ui channels */
    RGBA8UI,    /*  16-bit,          8-bit ui channels */
    RGBA16UI,   /*  64-bit,         16-bit ui channels */
    RGBA32UI,   /* 128-bit,         32-bit ui channels */

    RGB10A2UI,  /*  32-bit,         10-bit ui per channel, 2-bit ui alpha*/

    /* Floating-point formats */
    R16F,       /*  16-bit fp */
    R32F,       /*  32-bit fp */
    RG16F,      /*  32-bit,         16-bit fp channels */
    RG32F,      /*  64-bit,         32-bit fp channels */
    RGB16F,     /*  48-bit,         16-bit fp channels */
    RGB32F,     /*  96-bit,         32-bit fp channels */
    RGBA16F,    /*  64-bit,         16-bit fp channels */
    RGBA32F,    /* 128-bit,         32-bit fp channels */

    R11G11B10F, /*  32-bit,         11-bit fp RG, 10-bit fp B*/

    /* Compressed/nonlinear formats, use flags for further details */
    SRGB8,      /*  24-bit SRGB data*/
    SRGB8A8,    /*  24-bit SRGB data + 8-bit integer*/

    ASTC,
    BPTC,
    RGTC,
    S3TC,

    ETC1,

    /* Special data */
//    Stencil,         /*  8-bit integer, not a real format */
    Depth16,         /* 16-bit unsigned short */
    Depth24Stencil8, /* 32-bit data,           24-bit floating-point, 8-bit integer */
    Depth32F,         /* 32-bit floating-point */
    Depth32FStencil8, /* 32-bit floating-point, 8-bit stencil */

    MAX_PIXFMT,

    Depth, DepthStencil, /* Fakes */
};

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

    S3TC_1,
    S3TC_3,
    S3TC_5,
};

enum class BitFormat : u8
{
    Byte, ByteR,
    UByte, UByteR,
    UByte_332, UByte_233R,

    Short, ShortR,
    UShort, UShortR,
    UShort_4444, UShort_4444R,
    UShort_565, UShort_565R,
    UShort_5551, UShort_1555R,

    Int, IntR,
    UInt, UIntR,
    UInt_5999R,
    UInt_1010102, UInt_2101010R,

    Scalar_16,
    Scalar_32,
    Scalar_64,
    Scalar_11_11_10,

    UInt24_8,

    Scalar_32_Int_24_8,
};


FORCEDINLINE bool IsPixFmtCompressed(PixelFormat f)
{
    switch(f)
    {
    case PixelFormat::ASTC:
    case PixelFormat::S3TC:
    case PixelFormat::BPTC:
    case PixelFormat::RGTC:
    case PixelFormat::ETC1:
        return true;
    default:
        return false;
    }
}


using BitFmt = BitFormat;
using PixFmt = PixelFormat;
using PixFlg = PixelFlags;

struct CompFmt
{
    PixFmt base_fmt;
    PixFlg p_flags;
    CompFlags c_flags;
};

#if __cpp_constexpr >= 201304
#define CONSTEXPR_EXTENDED constexpr
#else
#define CONSTEXPR_EXTENDED
#endif

CONSTEXPR_EXTENDED
FORCEDINLINE BitFmt GetPreferredBitFmt(PixFmt fmt)
{
    using P = PixFmt;
    using B = BitFmt;

    switch(fmt)
    {
    case P::R16F:
    case P::RG16F:
    case P::RGB16F:
    case P::RGBA16F:
        return B::Scalar_16;

    case P::R32F:
    case P::RG32F:
    case P::RGB32F:
    case P::RGBA32F:
        return B::Scalar_32;

    case P::R8I:
    case P::RG8I:
    case P::RGB8I:
    case P::RGBA8I:
        return B::Byte;

    case P::R16I:
    case P::RG16I:
    case P::RGB16I:
    case P::RGBA16I:
        return B::Short;

    case P::R32I:
    case P::RG32I:
    case P::RGB32I:
    case P::RGBA32I:
        return B::Int;

    case P::RG16:
    case P::R16UI:
    case P::RG16UI:
    case P::RGB16UI:
    case P::RGBA16UI:
        return B::UShort;

    case P::R32UI:
    case P::RG32UI:
    case P::RGB32UI:
    case P::RGBA32UI:
        return B::UInt;

    case P::R3G3B2:
        return B::UByte_332;
    case P::RGB565:
        return B::UShort_565;
    case P::RGB5A1:
        return B::UShort_5551;
    case P::RGBA4:
        return B::UShort_4444;
    case P::RGB12:
    case P::RGB16:
    case P::RGBA12:
    case P::RGBA16:
        return B::UShort;

    case P::RGB10A2:
        return B::UInt_1010102;
    case P::R11G11B10F:
        return B::Scalar_11_11_10;
    case P::RGB9E5:
        return B::UInt_5999R;

    case P::Depth16:
        return B::UShort;
    case P::Depth24Stencil8:
        return B::UInt24_8;
    case  P::Depth32F:
        return B::Scalar_32;
    case P::Depth32FStencil8:
        return B::Scalar_32_Int_24_8;

    default:
        return B::UByte;
    }
}

CONSTEXPR_EXTENDED
FORCEDINLINE PixelFlags GetPixSampleType(PixFmt fmt)
{
    using F = PixFmt;

    switch(fmt)
    {
    case F::R8UI:
    case F::R16UI:
    case F::R32UI:
    case F::RG8UI:
    case F::RG16UI:
    case F::RG32UI:
    case F::RGB8UI:
    case F::RGB16UI:
    case F::RGB32UI:
    case F::RGB10A2UI:

    case F::RGBA8UI:
    case F::RGBA16UI:
    case F::RGBA32UI:
        return PixFlg::Unsigned;

    case F::R8I:
    case F::R16I:
    case F::R32I:
    case F::RG8I:
    case F::RG16I:
    case F::RG32I:
    case F::RGB8I:
    case F::RGB16I:
    case F::RGB32I:
    case F::RGBA8I:
    case F::RGBA16I:
    case F::RGBA32I:
        return PixFlg::Signed;

    default:
        return PixFlg::FloatingPoint;
    }
}

CONSTEXPR_EXTENDED
FORCEDINLINE PixCmp GetPixComponent(PixFmt fmt)
{
    using C = PixCmp;
    using F = PixFmt;

    switch(fmt)
    {
    case F::R8:
    case F::R8I:
    case F::R8UI:
    case F::R16:
    case F::R16I:
    case F::R16UI:
    case F::R32I:
    case F::R32UI:
    case F::R16F:
    case F::R32F:
        return C::R;
    case F::RG8:
    case F::RG8I:
    case F::RG8UI:
    case F::RG16:
    case F::RG16I:
    case F::RG16UI:
    case F::RG32I:
    case F::RG32UI:
    case F::RG16F:
    case F::RG32F:
        return C::RG;

    case F::R3G3B2:
    case F::RGB4:
    case F::RGB5:
    case F::RGB565:
    case F::SRGB8:
    case F::RGB10:
    case F::RGB12:
    case F::RGB9E5:
    case F::R11G11B10F:

    case F::RGB8:
    case F::RGB8I:
    case F::RGB8UI:
    case F::RGB16:
    case F::RGB16I:
    case F::RGB16UI:
    case F::RGB32I:
    case F::RGB32UI:
    case F::RGB16F:
    case F::RGB32F:
        return C::RGB;

    case F::RGBA2:
    case F::RGBA4:
    case F::RGB5A1:
    case F::RGBA12:
    case F::RGB10A2:
    case F::RGB10A2UI:
    case F::SRGB8A8:

    case F::RGBA8:
    case F::RGBA8I:
    case F::RGBA8UI:
    case F::RGBA16:
    case F::RGBA16I:
    case F::RGBA16UI:
    case F::RGBA32I:
    case F::RGBA32UI:
    case F::RGBA16F:
    case F::RGBA32F:
        return C::RGBA;

    case F::Depth16:
    case F::Depth32F:
        return C::Depth;

    case F::Depth24Stencil8:
    case F::Depth32FStencil8:
        return C::DepthStencil;

    default:
        return C::None;
    }
}

#if __cpp_constexpr >= 201304
template<PixFmt F, BitFmt B, PixelFlags Sample>
struct PixInfo
{
    static constexpr PixFmt format = F;
    static constexpr PixCmp components = GetPixComponent(F);
    static constexpr BitFmt bitfmt = B;
    static constexpr PixelFlags sample_type = Sample;
};

template<PixFmt F>
struct PixInfoDefault
        : PixInfo<F, GetPreferredBitFmt(F), GetPixSampleType(F)> { };
#endif

}

#undef CONSTEXPR_EXTENDED
