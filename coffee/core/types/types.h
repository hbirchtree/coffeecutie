#ifndef COFFEE_TYPES
#define COFFEE_TYPES

#include <stdint.h>
#include <map>
#include <vector>

#include "coffee/core/plat/cmemory.h"
#include "vector_types.h"
#include "composite_types.h"
#include "graphics_types.h"
#include "coffee/core/plat/plat_timers.h"

namespace Coffee{

template<typename T>
/*!
 * \brief For storing arrays
 */
struct chunk_mem
{
    szptr size = 0;
    T* d = nullptr;
};

/*!
 * \brief A template for version structures
 */
template<typename T> struct _cbasic_version
{
    T major;
    T minor;
    T revision;
};

/*!
 * \brief Expands an array contained in chunk_mem<T> types
 * \param mem
 * \param size
 */
template<typename T> static void coffee_mem_expand_array(
        chunk_mem<T> *mem, szptr size)
{
    szptr osize = mem->size;
    mem->size += size;
    mem->d = (T*)c_realloc(mem->d,sizeof(T)*mem->size);
    c_memclear(&mem->d[osize],sizeof(T)*(mem->size-osize));
}

}

#endif
