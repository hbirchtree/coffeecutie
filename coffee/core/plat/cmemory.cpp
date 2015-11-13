#include "cmemory.h"

#include <malloc.h>
#include <cstring>

namespace Coffee{
namespace CMem{

bool c_memcmp(c_cptr target, c_cptr cmp, szptr len)
{
    return memcmp(target,cmp,len)==0;
}

void c_memcpy(c_ptr dest, c_cptr source, szptr len)
{
    memcpy(dest,source,len);
}

void c_memclear(c_ptr start, szptr len){
    memset(start,0,len);
}

void *c_alloc(szptr datasize)
{
    return malloc(datasize);
}

void *c_realloc(c_ptr ptr, szptr datasize)
{
    return realloc(ptr,datasize);
}

void *c_calloc(szptr unit, szptr num)
{
    return calloc(unit,num);
}

cstring_w c_cpy_string(cstring str)
{
    cstring_w buf = (cstring_w)(c_alloc(strlen(str)+1));
    strcpy(buf,str);
    return buf;
}

bool c_strcmp(cstring s1, cstring s2)
{
    return strcmp(s1,s2)==0;
}

int c_strlen(cstring s)
{
    return strlen(s);
}

void c_free(c_ptr data)
{
    free(data);
}

}
}
