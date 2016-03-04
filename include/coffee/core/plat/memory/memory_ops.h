#pragma once

#include <cstring>
#include <malloc.h>

#include "../../types/tdef/integertypes.h"
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Mem{

/* Memory management */
FORCEDINLINE void CFree(c_ptr data)
{
    free(data);
}

FORCEDINLINE bool MemCmp(c_cptr target, c_cptr cmp, szptr len)
{
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

}
}