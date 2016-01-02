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
#include <string>
#include <stdexcept>

namespace Coffee{

template<typename T>
inline bool flag_eval(T flagcombo)
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

enum class ResourceAccess : uint8
{
    ExclusiveLocking,
    ReadOnly,
    WriteOnly,
    ReadWrite,
    Persistent,
};

enum class ResourceStorageClass : uint8
{
    Streaming = 0x1,
    Static = 0x2,
    Dynamic = 0x4,

    Write = 0x8,
    Read = 0x10,
    Copy = 0x18,
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

    LL_16, LL_16R,
    LL_32, LL_32R,
    LL_8, LL_8R,
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

enum class PixelFormat : uint8
{
    BGRA,
    RGBA,
    RGB,
    RG,
    RED,

    BGRA_i,
    RGBA_i,
    RGB_i,
    RG_i,
    RED_i,

    Stencil,
    Depth,
    DepthStencil,
};

template<typename T>
cstring ctypename(const T& t)
{
    return typeid(t).name();
}

}

#endif
