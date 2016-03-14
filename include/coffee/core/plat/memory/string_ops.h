#pragma once

#include "../platform_detect.h"

#ifdef COFFEE_ANDROID
namespace std{
#include <stdlib.h>
}
#else
#include <cstdlib>
#endif
#include <cstring>

#include "memory_ops.h"

namespace Coffee{
namespace Mem{

/* String utilities, includes wide strings */
FORCEDINLINE bool StrCmp(cstring s1, cstring s2)
{
    return strcmp(s1,s2)==0;
}

FORCEDINLINE int StrCmpI(cstring s1, cstring s2)
{
    return strcmp(s1,s2);
}

FORCEDINLINE cstring_w StrCat(cstring_w s1, cstring s2)
{
    return strcat(s1,s2);
}

FORCEDINLINE int StrLen(cstring s)
{
    return strlen(s);
}

FORCEDINLINE int StrLen(cwstring s)
{
    return wcslen(s);
}

FORCEDINLINE void StrCpy(cstring_w t, cstring s)
{
    strcpy(t,s);
}

namespace Search{
/* Low-level, yes! */
FORCEDINLINE cstring StrFind(cstring s1,cstring s2)
{
    return strstr(s1,s2);
}

FORCEDINLINE cstring ChrFind(cstring s, byte_t q)
{
    return strchr(s,q);
}
FORCEDINLINE cstring ChrFindR(cstring s, byte_t q)
{
    return strrchr(s,q);
}
FORCEDINLINE cstring ChrFindAny(cstring s, cstring q)
{
    return strpbrk(s,q);
}

FORCEDINLINE cstring_w Tokenize(cstring_w s, cstring q)
{
    return strtok(s,q);
}
}

namespace Convert
{
/* Conversion from string */
FORCEDINLINE int32 strtoint(cstring n,int base = 10, bool* ok = nullptr)
{
    if(!ok)
        return std::strtol(n,nullptr,base);
    char* t = nullptr;
    int32 v = std::strtol(n,&t,base);
    *ok = t>n;
    return v;
}
FORCEDINLINE uint32 strtouint(cstring n,int base = 10, bool* ok = nullptr)
{
    if(!ok)
        return std::strtoul(n,nullptr,base);
    char* t = nullptr;
    uint32 v = std::strtoul(n,&t,base);
    *ok = t>n;
    return v;
}
FORCEDINLINE int64 strtoll(cstring n,int base = 10, bool* ok = nullptr)
{
    if(!ok)
        return std::strtoll(n,nullptr,base);
    char* t = nullptr;
    int64 v = std::strtoll(n,&t,base);
    *ok = t>n;
    return v;
}
FORCEDINLINE uint64 strtoull(cstring n,int base = 10, bool* ok = nullptr)
{
    if(!ok)
        return std::strtoull(n,nullptr,base);
    char* t = nullptr;
    uint64 v = std::strtoull(n,&t,base);
    *ok = t>n;
    return v;
}
FORCEDINLINE scalar strtofscalar(cstring n, bool* ok = nullptr)
{
    if(!ok)
        return std::strtof(n,nullptr);
    char* t = nullptr;
    scalar v = std::strtof(n,&t);
    *ok = t>n;
    return v;
}
FORCEDINLINE bigscalar strtoscalar(cstring n, bool* ok = nullptr)
{
    if(!ok)
        return std::strtod(n,nullptr);
    char* t = nullptr;
    bigscalar v = std::strtod(n,&t);
    *ok = t>n;
    return v;
}

/* C-String conversion */
FORCEDINLINE cwstring_w NarrowWide(cstring str)
{
    size_t sz = strlen(str)+1;
    cwstring_w out = (cwstring_w)Calloc(sizeof(int16),sz);
    mbstowcs(out,str,sz);
    return out;
}
FORCEDINLINE cstring_w WideNarrow(cwstring str)
{
    size_t sz = wcslen(str)+1;
    cstring_w out = (cstring_w)Calloc(sizeof(int8),sz);
    wcstombs(out,str,sz);
    out[sz-1] = 0;
    return out;
}
}

}
}