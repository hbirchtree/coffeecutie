#ifndef COFFEE_BASETYPES_H
#define COFFEE_BASETYPES_H

#define C_FLAGS(type,utype) \
    inline type operator&(type lhs, type rhs){return (type)(((utype)lhs)&((utype)rhs));} \
    inline type operator|(type lhs, type rhs){return (type)(((utype)lhs)|((utype)rhs));} \
    inline type operator~(type val){return (type)(~((utype)val));} \
    inline type operator&=(type lhs, type rhs){return (type)(((utype)lhs)&((utype)rhs));} \
    inline type operator|=(type lhs, type rhs){return (type)(((utype)lhs)|((utype)rhs));}

//Thanks, Windows. Thanks.
#undef far
#undef near

#undef minor
#undef major

#include "../plat/platform_detect.h"

#include <typeinfo>
#include <stdint.h>
#include <map>
#include <vector>
#include <queue>
#include <list>
#include <mutex>
#include <atomic>
#include <functional>
#include <thread>
#include <string>
#include <stdexcept>

namespace Coffee{

template<typename T>
inline bool feval(T flagcombo)
{
    return (int)flagcombo;
}

//Integer types
using int8  = char;
using int16 = short;
using int32 = int;
#if defined(COFFEE_USE_LONGLONG_INT64)
using int64 = long long int;
#else
using int64 = long int;
#endif

using uint8  = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long int;

using scalar    = float;  /*!< Low-precision float value*/
using bigscalar = double; /*!< High-precision float value*/
using byte_t    = uint8;  /*!< System byte*/
using ubyte_t   = byte_t; /*!< System byte*/
using sbyte_t   = int8;   /*!< System unsigned byte*/

using ushort = uint16;

using cstring    = const sbyte_t*; /*!< Typical C-string*/
using cstring_w  = sbyte_t*; /*!< Writable C-string*/
using cwstring   = const wchar_t*; /*!< Wide C-string*/
using cwstring_w = wchar_t*; /*!< Wide, writable C-string*/

using szptr = size_t; /*!< Usable as offset into data*/

using CString   = std::string; /*!< Typical string object */
using CStdFault = std::runtime_error; /*!< Exception to be thrown by default */

using c_cptr = const void*;
using c_ptr  = void*;

using Mutex = std::mutex;
using Lock  = std::lock_guard<Mutex>;

template<typename T>
using ShPtr = std::shared_ptr<T>;

/*!
 * \brief A prototype for Coffee-managed main functions. Benefits to this is that Coffee will abstract away any platform-specific quirks to the main function, such as difference in arguments. (Eg. some platforms do not include the main executable while others do)
 */
using CoffeeMainWithArgs = int32(*)(int32,cstring_w*);

enum class ResourceAccess : uint16
{
    ExclusiveLocking = 0x1,

    ReadOnly = 0x10,
    WriteOnly = 0x20,
    ReadWrite = 0x30,

    Persistent = 0x40,
    Streaming = 0x80,

    Append = 0x100,

    Virtual = 0x200,
};

enum class TypeEnum : uint8
{
    UByte, Byte,
    UShort, Short,
    UInt, Int,
    ULL, LL,
    Scalar, BigScalar,
};

enum class BitFormat : uint8
{
    Byte, ByteR,
    UByte, UByteR,
    UByte_332, UByte_233R,

    Short, ShortR,
    UShort, UShortR,
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

    Scalar_32_Int_24_8,
};

enum class Severity : uint8
{
    Information,
    Debug,

    Low,
    Medium,
    High,

    Critical,
    Fatal,
};

enum class DebugType : uint8
{
    UndefinedBehavior,
    Information,
    Performance,
    Compliance,
    Deprecated,
    Compatibility,
    Marker,
};

enum class DebugComponent : uint8
{
    Core,
    GraphicsAPI,
    Audio,
    Interface,
    ShaderCompiler,
    LibraryLoader,
    Media,

    Extension,
};

enum class PixelFormat
{
    /* Formatting: [CHANNELS][NUMBITS][DATATYPE] */

    /* Irregular formats */

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

enum class PixelComponents
{
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

enum class PixelOperation
{
    SwapEndiannes,
    Alignment,
};

enum class Operator
{
    Add,
    Sub,
    RevSub,
    Min,
    Max,
};

enum class LogicOp
{
    CLEAR0 = 0x1,
    CLEAR1 = 0x2,

    AND = 0x4,
    NAND = 0x8,
    OR = 0x10,
    XOR = 0x20,
    NOR = 0x40,

    SRC_INVERSE = 0x80, /*Invert source*/
    DST_INVERSE = 0x100, /*Invert destination*/

    COPY = 0x200, /* Copy source */
    NOOP = 0x400, /* Copy destination */
};

enum class ValueComparison
{
    Never,
    Less,
    LEqual,
    Equal,
    NEqual,
    Greater,
    GEqual,
    Always,
};

enum class ValueHandling
{
    Zero,
    Keep,
    Replace,
    Increment,
    Decrement,
    WrapIncrement,
    WrapDecrement,
    Invert,
};

enum class ProfilingTerm
{
    ElapsedTime,
    ScreenSamples,
    ScreenPrimitives,
    TransformedPrimitives,
};

enum class Color
{
    Neutral,

    Pink,
    Red,
    FireRed,
    Orange,
    Yellow,
    Lime,
    LeafGreen,
    Green,
    Turquoise,
    Cyan,
    SkyBlue,
    Blue,
    DeepViolet,
    Purple,

    Brown,

    Black,
    LightGray,
    Gray,
    White,
};

template<typename T>
cstring ctypename(const T& t)
{
    return typeid(t).name();
}
template<typename T>
int64 ctypehash(const T& t)
{
    return typeid(t).hash_code();
}

C_FLAGS(PixelFlags,uint32);
C_FLAGS(ResourceAccess,uint32);
C_FLAGS(LogicOp,uint32);

}

#endif
