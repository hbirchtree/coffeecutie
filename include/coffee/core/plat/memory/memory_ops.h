#pragma once

#include <cstring>
#include <stdlib.h>

#include "../../types/tdef/integertypes.h"
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Mem{

template<typename T>
FORCEDINLINE void* FitIntegerInPtr(T in)
{
    static_assert(sizeof(T) <= sizeof(void*), "Type too large");

//    return C_FCAST<void*>(in);

    struct MemPack
    {
        union{
            void* ptr;
            u64 _u64;
            u32 _u32;
            u16 _u16;
            u8 _u8;
            i64 _i64;
            i32 _i32;
            i16 _i16;
            i8 _i8;
        };
    };

    constexpr bool sign = T(-1) < 0;

    MemPack mem;
    mem.ptr = nullptr;

#define M_OP(bits) \
    case sizeof(u ## bits): (sign) ? mem._i ## bits = in : mem._u ## bits = in; break;

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
FORCEDINLINE T ExtractIntegerPtr(void* ptr)
{
    return C_CAST<T>(C_FCAST<uintptr_t>(ptr));
}

/*!
 * \brief Remove offending bits from offset
 * \param alignment
 * \param off
 * \return
 */
FORCEDINLINE szptr AlignOffset(szptr alignment, szptr off)
{
    return off & ~(alignment);
}

/*!
 * \brief Align an offset by adding to the pointer.
 * \param off
 * \param alignment
 * \return
 */
FORCEDINLINE szptr AlignOffsetForward(szptr alignment, szptr off)
{
    return off + ((alignment - (off & (alignment - 1))) % alignment);
}

/* Memory management */
FORCEDINLINE void CFree(c_ptr data)
{
    free(data);
}

C_DEPRECATED FORCEDINLINE bool MemCmp(c_cptr target, c_cptr cmp, szptr len, int64* val = nullptr)
{
    if(val)
        return (*val = memcmp(target,cmp,len))==0;
    else
        return memcmp(target,cmp,len)==0;
}

C_DEPRECATED FORCEDINLINE void MemCpy(c_ptr dest, c_cptr source, szptr len)
{
    memcpy(dest,source,len);
}

C_DEPRECATED FORCEDINLINE void MemClear(c_ptr start, szptr len)
{
    memset(start,0,len);
}

/* Memory allocation */
C_DEPRECATED FORCEDINLINE void* Alloc(szptr datasize)
{
    return malloc(datasize);
}

C_DEPRECATED FORCEDINLINE void* Realloc(c_ptr ptr, szptr datasize)
{
    return realloc(ptr,datasize);
}

C_DEPRECATED FORCEDINLINE void* Calloc(szptr unit, szptr num)
{
    return calloc(unit,num);
}

template<typename T>
C_DEPRECATED FORCEDINLINE T* AllocT(szptr datasize)
{
    return static_cast<T*>(malloc(datasize));
}

template<typename T>
C_DEPRECATED FORCEDINLINE T* ReallocT(T* ptr, szptr datasize)
{
    return reinterpret_cast<T*>(realloc(ptr,datasize));
}

template<typename T>
C_DEPRECATED FORCEDINLINE T* CallocT(szptr unit, szptr num)
{
    return reinterpret_cast<T*>(calloc(unit,num));
}

template<typename T>
C_DEPRECATED FORCEDINLINE T* CallocPtrs(szptr num)
{
    return reinterpret_cast<T*>(calloc(sizeof(T),num));
}

}
}
