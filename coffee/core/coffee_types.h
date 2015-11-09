#ifndef COFFEE_TYPES
#define COFFEE_TYPES

#include <stdint.h>
#include <malloc.h>
#include <cstring>
#include <map>
#include <vector>

#include "coffee_vector_types.h"
#include "coffee_composite_types.h"
#include "coffee/core/plat/plat_timers.h"

namespace Coffee{

template<typename T>
/*!
 * \brief For storing arrays
 */
struct chunk_mem
{
    szptr   size    = 0;
    T*      d   = nullptr;
};

template<typename T>
/*!
 * \brief A template for version structures
 */
struct _cbasic_version
{
    T major     = 0;
    T minor     = 0;
    T revision  = 0;
};

/*!
 * \brief Primarily created to compare tag classes, it compares part of memory
 * \param target Typically a non-null-terminated string
 * \param cmp Typically what is compared to
 * \param len The length of the comparison
 * \return
 */
inline static bool coffee_cmp_memarea(cstring target, cstring cmp, szptr len)
{
    return memcmp(target,cmp,len)==0;
}

/*!
 * \brief Convenience function for copying strings
 * \param str
 * \return
 */
inline static cstring coffee_cpy_string(cstring str){
    cstring_w buf = reinterpret_cast<cstring_w>(malloc(strlen(str)+1));
    strcpy(buf,str);
    return buf;
}

/*!
 * \brief Convenience function for clearing memory
 * \param start
 * \param size
 */
inline static void coffee_mem_clear(void* start, szptr size){
    memset(start,0,size);
}

template<typename T>
/*!
 * \brief Expands an array contained in chunk_mem<T> types
 * \param mem
 * \param size
 */
static void coffee_mem_expand_array(chunk_mem<T> *mem, szptr size){
    szptr osize = mem->size;
    mem->size += size;
    mem->d = (T*)realloc(mem->d,sizeof(T)*mem->size);
    coffee_mem_clear(&mem->d[osize],sizeof(T)*(mem->size-osize));
}

}

#endif
