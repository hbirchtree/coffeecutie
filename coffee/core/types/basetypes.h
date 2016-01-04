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
typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long int            int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long int   uint64;

typedef float               scalar; /*!< Low-precision float value*/
typedef double              bigscalar; /*!< High-precision float value*/
typedef int8                byte_t; /*!< System byte*/
typedef uint8               ubyte_t; /*!< System unsigned byte*/

typedef uint16              ushort;

typedef const byte_t*       cstring; /*!< Typical C-string*/
typedef byte_t*             cstring_w; /*!< Writable C-string*/
typedef const wchar_t*      cwstring; /*!< Wide C-string*/
typedef wchar_t*            cwstring_w; /*!< Wide, writable C-string*/

typedef size_t              szptr; /*!< Usable as offset into data*/

typedef std::string         CString; /*!< Typical string object */
typedef std::runtime_error  CStdFault; /*!< Exception to be thrown by default */

typedef const void* c_cptr;
typedef void*       c_ptr;

/*!
 * \brief A prototype for Coffee-managed main functions. Benefits to this is that Coffee will abstract away any platform-specific quirks to the main function, such as difference in arguments. (Eg. some platforms do not include the main executable while others do)
 */
typedef int32 (*CoffeeMainWithArgs)(int32,byte_t**);

enum class ResourceAccess : uint16
{
    ExclusiveLocking = 0x1,

    ReadOnly = 0x10,
    WriteOnly = 0x20,
    ReadWrite = 0x40,

    Persistent = 0x80,
    Streaming = 0x100,
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
    Byte_2, Byte_2R,
    Byte_332, Byte_233R,

    Short_565, Short_565R,
    Short_4, Short_4R,
    Short_5551, Short_1555R,

    Int_8, Int_8R,
    Int_1010102, Int_2101010R,

    LL_8, LL_8R,
    LL_16, LL_16R,
    LL_32, LL_32R,
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
};

enum class PixelFormat
{
    /* Formatting: [CHANNELS][NUMBITS][DATATYPE] */

    /* Irregular formats */

    RGB4,       /*  12-bit integer,          4-bit per channel*/
    RGB5,       /*  15-bit integer,          5-bit per channel*/
    RGB10,      /*  30-bit integer,         10-bit per channel*/
    RGB12,      /*  36-bit integer,         12-bit per channel*/

    RGBA2,      /*   8-bit integer,          2-bit per channel */
    RGB5A1,     /*  16-bit integer,          5-bit per channel, 1-bit alpha*/
    RGB10A2,    /*  32-bit integer,         10-bit per channel, 2-bit alpha*/
    RGB10A2UI,  /*  32-bit integer,         10-bit unsigned per channel, 2-bit unsigned alpha*/
    RGBA12,     /*  48-bit integer,         12-bit per channel */

    R3G3B2,     /*   8-bit integer,          3-bit R, 3-bit G, 2-bit B */
    RGB565,     /*  16-bit integer,          5-bit R, 6-bit G, 5-bit B */
    RGB9E5,     /*  32-bit integer,          9-bit integer channel RGB, 5-bit extra */
    R11G11B10F, /*  11-bit float R, 11-bit float G, 10-bit float B */

    /* Regular formats, R, RG, RGB, RGBA */

    R8I,        /*   8-bit integer */
    R8UI,       /*   8-bit unsigned integer */
    R16I,       /*  16-bit integer */
    R16UI,      /*  16-bit unsigned integer */
    R16F,       /*  16-bit floating-point */
    R32I,       /*  32-bit integer */
    R32UI,      /*  32-bit unsigned integer */
    R32F,       /*  32-bit floating-point */

    RG8I,       /*  16-bit integer,          8-bit integer channels */
    RG8UI,      /*  16-bit unsigned integer, 8-bit unsigned integer channels */
    RG16I,      /*  32-bit integer,         16-bit integer channels */
    RG16UI,     /*  32-bit unsigned integer,16-bit unsigned integer channels */
    RG16F,      /*  32-bit floating-point,  16-bit floating-point channels */
    RG32I,      /*  64-bit integer,         32-bit integer channels */
    RG32UI,     /*  64-bit unsigned integer,32-bit unsigned integer channels */
    RG32F,      /*  64-bit floating-point,  32-bit floating-point channels */

    RGB8I,       /* 24-bit integer,          8-bit integer channels */
    RGB8UI,      /* 24-bit integer,          8-bit unsigned integer channels */
    RGB16I,      /* 48-bit integer,         16-bit integer channels */
    RGB16UI,     /* 48-bit unsigned integer,16-bit unsigned integer channels */
    RGB16F,      /* 48-bit floating-point,  16-bit floating-point channels */
    RGB32I,      /* 96-bit integer,         32-bit integer channels */
    RGB32UI,     /* 96-bit unsigned integer,32-bit unsigned integer channels */
    RGB32F,      /* 96-bit floating-point,  32-bit floating-point channels */

    RGBA8I,     /*  16-bit integer,          8-bit integer channels */
    RGBA8UI,    /*  16-bit unsigned integer, 8-bit unsigned integer channels */
    RGBA16I,    /*  64-bit integer,         16-bit integer channels */
    RGBA16UI,   /*  64-bit unsigned integer,16-bit unsigned integer channels */
    RGBA16F,    /*  64-bit floating-point,  16-bit floating-point channels */
    RGBA32I,    /* 128-bit integer,         32-bit integer channels */
    RGBA32UI,   /* 128-bit unsigned integer,32-bit unsigned integer channels */
    RGBA32F,    /* 128-bit floating-point,  32-bit floating-point channels */

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
    ThroughputMetric,
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

}

#endif
