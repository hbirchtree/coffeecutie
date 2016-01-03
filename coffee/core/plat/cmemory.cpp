#include "cmemory.h"

#include <malloc.h>
#include <cstring>
#include <cwchar>

namespace Coffee{
namespace CMem{

cstring c_gets(cstring_w target, int32 size, FILE *strm)
{
    return fgets(target,size,strm);
}

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

int c_strlen(cwstring s)
{
    return wcslen(s);
}

void c_free(c_ptr data)
{
    free(data);
}

cwstring_w c_str_wideconvert(cstring str)
{
    size_t sz = strlen(str)+1;
    cwstring_w out = (cwstring_w)c_calloc(sizeof(int16),sz);
    mbstowcs(out,str,sz);
    return out;
}

cstring_w c_str_narrowconvert(cwstring str)
{
    size_t sz = wcslen(str)+1;
    cstring_w out = (cstring_w)c_calloc(sizeof(int8),sz);
    wcstombs(out,str,sz);
    out[sz-1] = 0;
    return out;
}

CString c_str_replace(const CString &target, const CString &query, const CString &replacement)
{
    CString out = target;
    for(size_t pos=0;;pos+=replacement.length())
    {
        pos = out.find(query,pos);
        if(pos==std::string::npos)
            break;
        out.erase(pos,query.length());
        out.insert(pos,replacement);
    }
    return out;
}

cstring_w c_str_replace(cstring target, cstring query, cstring replacement)
{
    CString res = c_str_replace(CString(target),CString(query),CString(replacement));
    cstring_w out = (cstring_w)c_alloc(res.size()+1);
    res.copy(out,res.size());
    out[res.size()] = 0;
    return out;
}

cstring_w c_strcat(cstring_w s1, cstring s2)
{
    return strcat(s1,s2);
}

bool c_strstr(cstring s1, cstring s2)
{
    return strstr(s1,s2);
}

void c_puts(FILE *strm, cstring output)
{
    fputs(output,strm);
}

}
}
