#pragma once

#include "../platform_detect.h"

#include <cstdlib>
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

FORCEDINLINE bool StrICmp(cstring s1, cstring s2)
{
#if defined(COFFEE_UNIXPLAT)
    return strcasecmp(s1, s2)==0;
#elif defined(COFFEE_WINDOWS)
    return stricmp(s1, s2);
#endif
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

#define STRTON_FUNCTION(type, name, converter) \
    FORCEDINLINE type name(cstring n,int base = 10, bool* ok = nullptr) \
{ \
    if(!ok) \
    return ::converter(n,nullptr,base); \
    char* t = nullptr; \
    type v = ::converter(n,&t,base); \
    *ok = t>n; \
    return v; \
}

#define STRTOFN_FUNCTION(type, name, converter) \
    FORCEDINLINE type name(cstring n, bool* ok = nullptr) \
{ \
    if(!ok) \
    return ::converter(n,nullptr); \
    char* t = nullptr; \
    type v = ::converter(n,&t); \
    *ok = t>n; \
    return v; \
}

namespace Convert
{
/* Conversion from string */
STRTON_FUNCTION(int32, strtoint, strtol)
STRTON_FUNCTION(uint32, strtouint, strtoul)
STRTON_FUNCTION(int64, strtoll, strtoll)
STRTON_FUNCTION(uint64, strtoull, strtoull)

STRTOFN_FUNCTION(scalar, strtofscalar, strtof)
STRTOFN_FUNCTION(bigscalar, strtoscalar, strtod)

extern bool strtobool(cstring n, bool *valid = nullptr);

/* C-String conversion */
extern cwstring_w NarrowWide(cstring str);
extern cstring_w WideNarrow(cwstring str);

/* This is a workaround till C++17 constexpr if is made available. */
FORCEDINLINE cstring_w WideNarrow(cstring) { return nullptr; }

}

}
}

#undef STRTON_FUNCTION
#undef STRTOFN_FUNCTION
