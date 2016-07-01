#pragma once

#include <cstring>
#include <stdlib.h>

#include "../../types/tdef/integertypes.h"
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Mem{

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
    return (T*)malloc(datasize);
}

template<typename T>
FORCEDINLINE T* ReallocT(T* ptr, szptr datasize)
{
    return (T*)realloc(ptr,datasize);
}

template<typename T>
FORCEDINLINE T* CallocT(szptr unit, szptr num)
{
    return (T*)calloc(unit,num);
}

template<typename T>
FORCEDINLINE T* CallocPtrs(szptr num)
{
    return (T*)calloc(sizeof(T),num);
}

}
}
