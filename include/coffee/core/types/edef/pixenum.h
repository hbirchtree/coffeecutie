#pragma once

#include "enumfun.h"
#include "../tdef/integertypes.h"

#undef None

namespace Coffee{

enum class PixelFlags
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

enum class PixelComponents
{
    None,

    R,
    G,
    B,
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

enum class PixelOperation
{
    SwapEndiannes,
    Alignment,
};

enum class PixelFormat
{
    /* Formatting: [CHANNELS][NUMBITS][DATATYPE] */

    /* Irregular formats */

    RGBA8,      /*  32-bit,          8-bit per channel*/

    RGB4UI,     /*  12-bit,          4-bit per channel*/
    RGB5UI,     /*  15-bit,          5-bit per channel*/
    RGB10,      /*  30-bit,         10-bit per channel*/
    RGB12,      /*  36-bit,         12-bit per channel*/

    RGBA2,      /*   8-bit,          2-bit per channel */
    RGB5A1UI,   /*  16-bit,          5-bit per channel, 1-bit alpha*/
    RGB10A2I,   /*  32-bit,         10-bit per channel, 2-bit alpha*/
    RGB10A2UI,  /*  32-bit,         10-bit ui per channel, 2-bit ui alpha*/
    RGBA12,     /*  48-bit,         12-bit per channel */

    R3G3B2UI,   /*   8-bit,          3-bit R, 3-bit G, 2-bit B */
    RGB565UI,   /*  16-bit,          5-bit R, 6-bit G, 5-bit B */
    RGB9E5UI,   /*  32-bit,          9-bit special fp RGB, 5-bit ui extra */
    R11G11B10F, /*  31-bit,         11-bit fp RG, 10-bit fp B*/

    /* Regular formats, R, RG, RGB, RGBA */

    R8I,        /*   8-bit i*/
    R8UI,       /*   8-bit ui*/
    R16I,       /*  16-bit i */
    R16UI,      /*  16-bit ui */
    R16F,       /*  16-bit fp */
    R32I,       /*  32-bit i */
    R32UI,      /*  32-bit ui */
    R32F,       /*  32-bit fp */

    RG8I,       /*  16-bit,          8-bit i channels */
    RG8UI,      /*  16-bit,          8-bit ui channels */
    RG16I,      /*  32-bit,         16-bit i channels */
    RG16UI,     /*  32-bit,         16-bit ui channels */
    RG16F,      /*  32-bit,         16-bit fp channels */
    RG32I,      /*  64-bit,         32-bit i channels */
    RG32UI,     /*  64-bit,         32-bit ui channels */
    RG32F,      /*  64-bit,         32-bit fp channels */

    RGB8I,      /*  24-bit,          8-bit i channels */
    RGB8UI,     /*  24-bit,          8-bit ui channels */
    RGB16I,     /*  48-bit,         16-bit i channels */
    RGB16UI,    /*  48-bit,         16-bit ui channels */
    RGB16F,     /*  48-bit,         16-bit fp channels */
    RGB32I,     /*  96-bit,         32-bit i channels */
    RGB32UI,    /*  96-bit,         32-bit ui channels */
    RGB32F,     /*  96-bit,         32-bit fp channels */

    RGBA8I,     /*  16-bit,          8-bit i channels */
    RGBA8UI,    /*  16-bit,          8-bit ui channels */
    RGBA16I,    /*  64-bit,         16-bit i channels */
    RGBA16UI,   /*  64-bit,         16-bit ui channels */
    RGBA16F,    /*  64-bit,         16-bit fp channels */
    RGBA32I,    /* 128-bit,         32-bit i channels */
    RGBA32UI,   /* 128-bit,         32-bit ui channels */
    RGBA32F,    /* 128-bit,         32-bit fp channels */

    /* Compressed/nonlinear formats, use flags for further details */
    SRGB8,      /*  24-bit SRGB data*/
    SRGB8A8,    /*  24-bit SRGB data + 8-bit integer*/

    ASTC,
    BPTC,
    RGTC,
    S3TC,

    /* Special data */
    Stencil,      /*  8-bit integer */
    Depth,        /* 32-bit floating-point */
    DepthStencil, /* 32-bit data,           24-bit floating-point, 8-bit integer */
};

}
