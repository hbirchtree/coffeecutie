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

extern void* c_calloc(szptr unit, szptr num);

extern bool c_strcmp(cstring s1, cstring s2);

extern int c_strlen(cstring s);

extern cstring_w c_cpy_string(cstring str);

}

using namespace CMem;

}

#endif
