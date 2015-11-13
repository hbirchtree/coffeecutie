#ifndef COFFEE_TYPES
#define COFFEE_TYPES

#include <stdint.h>
#include <map>
#include <vector>

#include "plat/cmemory.h"
#include "coffee_vector_types.h"
#include "coffee_composite_types.h"
#include "plat/plat_timers.h"

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
