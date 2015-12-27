#ifndef COFFEE_PLAT_MEMORY_H
#define COFFEE_PLAT_MEMORY_H

#include "coffee/core/types/basetypes.h"

namespace Coffee{
namespace CMem{

extern void c_free(c_ptr data);

extern bool c_memcmp(c_cptr target, c_cptr cmp, szptr len);

extern void c_memcpy(c_ptr dest, c_cptr source, szptr len);

extern void c_memclear(c_ptr start, szptr len);

extern void* c_alloc(szptr datasize);

extern void* c_realloc(c_ptr ptr, szptr datasize);
/*!
 * \brief Should provide a 0-initialized chunk of memory
 * \param unit
 * \param num
 * \return
 */
extern void* c_calloc(szptr unit, szptr num);

extern bool c_strcmp(cstring s1, cstring s2);

extern cstring_w c_strcat(cstring_w s1, cstring s2);

extern int c_strlen(cstring s);

extern int c_strlen(cwstring s);

extern cstring_w c_cpy_string(cstring str);

extern cwstring_w c_str_wideconvert(cstring str);

extern cstring_w c_str_narrowconvert(cwstring str);

extern cstring_w c_str_replace(
        cstring target, cstring query,
        cstring replacement);

extern CString c_str_replace(
        const CString &target, const CString &query,
        const CString &replacement);

}

using namespace CMem;

}

#endif
