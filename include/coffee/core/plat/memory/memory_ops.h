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
//    struct MemPack
//    {
//        void* ptr;
//        u64 _u64;
//        u32 _u32;
//        u16 _u16;
//        u8 _u8;

//        i64 _i64;
//        i32 _i32;
//        i16 _i16;
//        i8 _i8;
//    };

//    MemPack m;
//    m.ptr = ptr;

//    constexpr bool sign = T(-1) < 0;

//#define M_OP(bits) \
//    case sizeof(u ## bits): return (sign) ? T(m._i ## bits) : T(m._u ## bits);

//    switch(sizeof(T))
//    {
//    M_OP(64);
//    M_OP(32);
//    M_OP(16);
//    M_OP(8);
//    default:
//        return T(0);
//    }
//#undef M_OP
}

FORCEDINLINE szptr AlignOffset(szptr alignment, szptr off)
{
    return off & ~(alignment);
}

/* Memory management */
FORCEDINLINE void CFree(c_ptr data)
{
    free(data);
}

FORCEDINLINE bool MemCmp(c_cptr target, c_cptr cmp, szptr len, int64* val = nullptr)
{
    if(val)
        return (*val = memcmp(target,cmp,len))==0;
    else
        return memcmp(target,cmp,len)==0;
}

FORCEDINLINE void MemCpy(c_ptr dest, c_cptr source, szptr len)
{
    memcpy(dest,source,len);
}

FORCEDINLINE void MemClear(c_ptr start, szptr len)
{
    memset(start,0,len);
}

/* Memory allocation */
FORCEDINLINE void* Alloc(szptr datasize)
{
    return malloc(datasize);
}

FORCEDINLINE void* Realloc(c_ptr ptr, szptr datasize)
{
    return realloc(ptr,datasize);
}

FORCEDINLINE void* Calloc(szptr unit, szptr num)
{
    return calloc(unit,num);
}

template<typename T>
FORCEDINLINE T* AllocT(szptr datasize)
{
    return static_cast<T*>(malloc(datasize));
}

template<typename T>
FORCEDINLINE T* ReallocT(T* ptr, szptr datasize)
{
    return static_cast<T*>(realloc(ptr,datasize));
}

template<typename T>
FORCEDINLINE T* CallocT(szptr unit, szptr num)
{
    return static_cast<T*>(calloc(unit,num));
}

template<typename T>
FORCEDINLINE T* CallocPtrs(szptr num)
{
    return static_cast<T*>(calloc(sizeof(T),num));
}

}
}
