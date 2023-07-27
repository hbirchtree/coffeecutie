#pragma once

#include <cstring>
#include <stdlib.h>

#include <peripherals/identify/identify.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>

#if defined(COFFEE_UNIXPLAT)
#include <unistd.h>
#endif

namespace libc {
namespace ptr {

template<typename T>
FORCEDINLINE void* put_value(T in)
{
    using namespace ::libc_types;

    static_assert(sizeof(T) <= sizeof(void*), "Type too large");

    //    return C_FCAST<void*>(in);

    struct MemPack
    {
        union
        {
            void* ptr;
            u64   _u64;
            u32   _u32;
            u16   _u16;
            u8    _u8;
            i64   _i64;
            i32   _i32;
            i16   _i16;
            i8    _i8;
        };
    };

    constexpr bool sign = T(-1) < 0;

    MemPack mem;
    mem.ptr = nullptr;

#define M_OP(bits)                                      \
    case sizeof(u##bits):                               \
        (sign) ? mem._i##bits = in : mem._u##bits = in; \
        break;

    switch(sizeof(in))
    {
        M_OP(64);
        M_OP(32);
        M_OP(16);
        M_OP(8);
    }
#undef M_OP

    return mem.ptr;
}

template<typename T>
FORCEDINLINE T extract(void* ptr)
{
    using namespace ::type_safety;

    static_assert(sizeof(T) <= sizeof(void*), "Type too large");

    return C_CAST<T>(C_FCAST<uintptr_t>(ptr));
}

} // namespace ptr

namespace align {

using namespace ::libc_types;

enum AlignmentDirection
{
    dir_default, /*!< Will mask the offset backwards */
    dir_forward  /*!< Will mask the offset forwards */
};

template<
    AlignmentDirection Dir                             = dir_default,
    typename std::enable_if<Dir == dir_default>::type* = nullptr>
FORCEDINLINE szptr align(szptr alignment, szptr off)
{
    return off & ~(alignment);
}

template<
    AlignmentDirection Dir,
    typename std::enable_if<Dir == dir_forward>::type* = nullptr>
FORCEDINLINE szptr align(szptr alignment, szptr off)
{
    return off + ((alignment - (off & (alignment - 1))) % alignment);
}

} // namespace align
namespace mem {

FORCEDINLINE libc_types::u32 page_size()
{
    using namespace libc_types::size_literals;
#if defined(COFFEE_UNIXPLAT)
    libc_types::i64 pagesize = ::sysconf(_SC_PAGESIZE);

    return C_FCAST<libc_types::u32>(pagesize > 0 ? pagesize : 4_kB);
#else
    return 4_kB;
#endif
}

}
} // namespace libc
