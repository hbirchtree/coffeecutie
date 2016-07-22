#pragma once

#include "../platform_detect.h"

//#ifdef COFFEE_ANDROID
//#include <stdlib.h>
//#else
#include <cstdlib>
//#endif
#include <cstring>
#include <cwchar>

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

FORCEDINLINE szptr StrLen(cstring s)
{
    if(!s)
	return 0;
    return strlen(s);
}

FORCEDINLINE szptr StrLen(cwstring s)
{
    if(!s)
	return 0;
    return wcslen(s);
}

FORCEDINLINE void StrCpy(cstring_w t, cstring s)
{
    strcpy(t,s);
}

namespace Search{
/* Low-level, yes! */

	/* The byte overloads */
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

/* Wide strings */
FORCEDINLINE cwstring StrFind(cwstring s1, cwstring s2)
{
	return wcsstr(s1, s2);
}
FORCEDINLINE cwstring ChrFind(cwstring s1, wbyte_t s2)
{
	return wcschr(s1, s2);
}
FORCEDINLINE cwstring ChrFindR(cwstring s1, wbyte_t s2)
{
	return wcsrchr(s1, s2);
}
FORCEDINLINE cwstring ChrFindAny(cwstring s1, cwstring s2)
{
	return wcspbrk(s1, s2);
}

}

namespace Convert
{
/* Conversion from string */
FORCEDINLINE int32 strtoint(cstring n,int base = 10, bool* ok = nullptr)
{
    if(!ok)
        return ::strtol(n,nullptr,base);
    char* t = nullptr;
    int32 v = ::strtol(n,&t,base);
    *ok = t>n;
    return v;
}
FORCEDINLINE uint32 strtouint(cstring n,int base = 10, bool* ok = nullptr)
{
    if(!ok)
        return ::strtoul(n,nullptr,base);
    char* t = nullptr;
    uint32 v = ::strtoul(n,&t,base);
    *ok = t>n;
    return v;
}
FORCEDINLINE int64 strtoll(cstring n,int base = 10, bool* ok = nullptr)
{
    if(!ok)
        return ::strtoll(n,nullptr,base);
    char* t = nullptr;
    int64 v = ::strtoll(n,&t,base);
    *ok = t>n;
    return v;
}
FORCEDINLINE uint64 strtoull(cstring n,int base = 10, bool* ok = nullptr)
{
    if(!ok)
        return ::strtoull(n,nullptr,base);
    char* t = nullptr;
    uint64 v = ::strtoull(n,&t,base);
    *ok = t>n;
    return v;
}
FORCEDINLINE scalar strtofscalar(cstring n, bool* ok = nullptr)
{
    if(!ok)
        return ::strtof(n,nullptr);
    char* t = nullptr;
    scalar v = ::strtof(n,&t);
    *ok = t>n;
    return v;
}
FORCEDINLINE bigscalar strtoscalar(cstring n, bool* ok = nullptr)
{
    if(!ok)
        return ::strtod(n,nullptr);
    char* t = nullptr;
    bigscalar v = ::strtod(n,&t);
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

/* This is a workaround till C++17 constexpr if is made available. */
FORCEDINLINE cstring_w WideNarrow(cstring str)
{
	return nullptr;
}
}

}
}
