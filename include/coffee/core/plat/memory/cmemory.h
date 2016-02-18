#ifndef COFFEE_PLAT_MEMORY_H
#define COFFEE_PLAT_MEMORY_H

#include "../platform_detect.h"

#ifndef COFFEE_WINDOWS
#include <arpa/inet.h>
#else
//#include <WinSock2.h>
#endif

#include <cctype>
#include <algorithm>
#include <malloc.h>
#include <cstring>
#include <cwchar>
#include <cstdio>

#include "../../coffee_mem_macros.h"
#include "../../types/basetypes.h"

namespace Coffee{

static FILE* DefaultDebugOutputPipe = stderr;
static FILE* DefaultPrintOutputPipe = stderr;

namespace CMem{

/* Endian conversion */
#ifndef COFFEE_WINDOWS
FORCEDINLINE uint32 ForeignEndian32(uint32 i)
{
    return htonl(i);
}

FORCEDINLINE uint16 ForeignEndian16(uint16 i)
{
    return htons(i);
}

FORCEDINLINE uint32 NativeEndian32(uint32 i)
{
    return ntohl(i);
}

FORCEDINLINE uint16 NativeEndian16(uint16 i)
{
    return ntohs(i);
}
#endif

/* Input/output */
FORCEDINLINE cstring CGets(cstring_w target, int32 size, FILE* strm)
{
    return fgets(target,size,strm);
}

FORCEDINLINE void CPuts(FILE* strm, cstring output)
{
    fputs(output,strm);
}

/* Memory management */
FORCEDINLINE void CFree(c_ptr data)
{
    free(data);
}

FORCEDINLINE bool CMemCmp(c_cptr target, c_cptr cmp, szptr len)
{
    return memcmp(target,cmp,len)==0;
}

FORCEDINLINE void CMemCpy(c_ptr dest, c_cptr source, szptr len)
{
    memcpy(dest,source,len);
}

FORCEDINLINE void CMemClear(c_ptr start, szptr len)
{
    memset(start,0,len);
}

FORCEDINLINE void* Alloc(szptr datasize)
{
    return malloc(datasize);
}

FORCEDINLINE void* CRealloc(c_ptr ptr, szptr datasize)
{
    return realloc(ptr,datasize);
}

FORCEDINLINE void* CCalloc(szptr unit, szptr num)
{
    return calloc(unit,num);
}

/* String utilities, includes wide strings */
FORCEDINLINE bool CStrCmp(cstring s1, cstring s2)
{
    return strcmp(s1,s2)==0;
}

FORCEDINLINE int CStrCompare(cstring s1, cstring s2)
{
    return strcmp(s1,s2);
}

FORCEDINLINE cstring_w CStrCat(cstring_w s1, cstring s2)
{
    return strcat(s1,s2);
}

FORCEDINLINE cstring CStrFind(cstring s1,cstring s2)
{
    return strstr(s1,s2);
}

FORCEDINLINE int CStrLen(cstring s)
{
    return strlen(s);
}

FORCEDINLINE int CStrLen(cwstring s)
{
    return wcslen(s);
}

FORCEDINLINE cstring_w CCpyStr(cstring str)
{
    cstring_w buf = (cstring_w)(Alloc(strlen(str)+1));
    strcpy(buf,str);
    return buf;
}

/* Higher-level string utilities */
FORCEDINLINE CString CStrReplace(
        const CString &target, const CString &query,
        const CString &replacement)
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

FORCEDINLINE cstring_w CStrReplace(
        cstring target, cstring query,
        cstring replacement)
{
    CString res = CStrReplace(CString(target),CString(query),CString(replacement));
    cstring_w out = (cstring_w)Alloc(res.size()+1);
    res.copy(out,res.size());
    out[res.size()] = 0;
    return out;
}

namespace Convert
{
/*TODO: Add more conversions!*/
FORCEDINLINE int32 strtoint(cstring n)
{
    return atoi(n);
}
FORCEDINLINE int64 strtoll(cstring n)
{
    return atoll(n);
}
FORCEDINLINE bigscalar strtoscalar(cstring n)
{
    return atof(n);
}

FORCEDINLINE cwstring_w NarrowWide(cstring str)
{
    size_t sz = strlen(str)+1;
    cwstring_w out = (cwstring_w)CCalloc(sizeof(int16),sz);
    mbstowcs(out,str,sz);
    return out;
}
FORCEDINLINE cstring_w WideNarrow(cwstring str)
{
    size_t sz = wcslen(str)+1;
    cstring_w out = (cstring_w)CCalloc(sizeof(int8),sz);
    wcstombs(out,str,sz);
    out[sz-1] = 0;
    return out;
}
}

namespace StrUtil
{
/*TODO: Add strok() and other string processing utilities here. They are nice to have.*/

/* Reference: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring */
FORCEDINLINE CString& ltrim(CString& s)
{
    s.erase(s.begin(),
            std::find_if(
                s.begin(),
                s.end(),
                std::not1(
                    std::ptr_fun<int,int>(std::isspace)
                    )
                )
            );
    return s;
}
FORCEDINLINE CString& rtrim(CString& s)
{
    s.erase(std::find_if(
                s.rbegin(),
                s.rend(),
                std::not1(
                    std::ptr_fun<int,int>(std::isspace)
                    )
                ).base(),
            s.end()
            );
    return s;
}
FORCEDINLINE CString& trim(CString& s)
{
    return ltrim(rtrim(s));
}
FORCEDINLINE CString& zeroltrim(CString& s)
{
    s.erase(0,s.find_first_not_of('0'));
    return s;
}
FORCEDINLINE CString& zerortrim(CString& s)
{
    s.erase(s.find_last_not_of('0')+1,s.size());
    return s;
}
FORCEDINLINE CString& zerotrim(CString& s)
{
    return zeroltrim(zerortrim(s));
}

}

}

using namespace CMem;

}

#endif
