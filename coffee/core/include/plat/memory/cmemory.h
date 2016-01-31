#ifndef COFFEE_PLAT_MEMORY_H
#define COFFEE_PLAT_MEMORY_H

#include <arpa/inet.h>
#include <malloc.h>
#include <cstring>
#include <cwchar>

#include <coffee_mem_macros.h>
#include <types/basetypes.h>

namespace Coffee{
namespace CMem{

/* Endian conversion */
inline C_FORCE_INLINE uint32 ForeignEndian32(uint32 i)
{
    return htonl(i);
}

inline C_FORCE_INLINE uint16 ForeignEndian16(uint16 i)
{
    return htons(i);
}

inline C_FORCE_INLINE uint32 NativeEndian32(uint32 i)
{
    return ntohl(i);
}

inline C_FORCE_INLINE uint16 NativeEndian16(uint16 i)
{
    return ntohs(i);
}

/* Input/output */
inline C_FORCE_INLINE cstring CGets(cstring_w target, int32 size, FILE* strm)
{
    return fgets(target,size,strm);
}

inline C_FORCE_INLINE void CPuts(FILE* strm, cstring output)
{
    fputs(output,strm);
}

/* Memory management */
inline C_FORCE_INLINE void CFree(c_ptr data)
{
    free(data);
}

inline C_FORCE_INLINE bool CMemCmp(c_cptr target, c_cptr cmp, szptr len)
{
    return memcmp(target,cmp,len)==0;
}

inline C_FORCE_INLINE void CMemCpy(c_ptr dest, c_cptr source, szptr len)
{
    memcpy(dest,source,len);
}

inline C_FORCE_INLINE void CMemClear(c_ptr start, szptr len)
{
    memset(start,0,len);
}

inline C_FORCE_INLINE void* Alloc(szptr datasize)
{
    return malloc(datasize);
}

inline C_FORCE_INLINE void* CRealloc(c_ptr ptr, szptr datasize)
{
    return realloc(ptr,datasize);
}

inline C_FORCE_INLINE void* CCalloc(szptr unit, szptr num)
{
    return calloc(unit,num);
}

/* String utilities, includes wide strings */
inline C_FORCE_INLINE bool CStrCmp(cstring s1, cstring s2)
{
    return strcmp(s1,s2)==0;
}

inline C_FORCE_INLINE cstring_w CStrCat(cstring_w s1, cstring s2)
{
    return strcat(s1,s2);
}

inline C_FORCE_INLINE bool CStrFind(cstring s1,cstring s2)
{
    return strstr(s1,s2);
}

inline C_FORCE_INLINE int CStrLen(cstring s)
{
    return strlen(s);
}

inline C_FORCE_INLINE int CStrLen(cwstring s)
{
    return wcslen(s);
}

inline C_FORCE_INLINE cstring_w CCpyStr(cstring str)
{
    cstring_w buf = (cstring_w)(Alloc(strlen(str)+1));
    strcpy(buf,str);
    return buf;
}

inline C_FORCE_INLINE cwstring_w CStrWConvert(cstring str)
{
    size_t sz = strlen(str)+1;
    cwstring_w out = (cwstring_w)CCalloc(sizeof(int16),sz);
    mbstowcs(out,str,sz);
    return out;
}

inline C_FORCE_INLINE cstring_w CStrNConvert(cwstring str)
{
    size_t sz = wcslen(str)+1;
    cstring_w out = (cstring_w)CCalloc(sizeof(int8),sz);
    wcstombs(out,str,sz);
    out[sz-1] = 0;
    return out;
}

/* Higher-level string utilities */
inline C_FORCE_INLINE CString CStrReplace(
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

inline C_FORCE_INLINE cstring_w CStrReplace(
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
inline C_FORCE_INLINE int32 strtoint(cstring n)
{
    return atoi(n);
}
inline C_FORCE_INLINE int64 strtoll(cstring n)
{
    return atoll(n);
}
inline C_FORCE_INLINE bigscalar strtoscalar(cstring n)
{
    return atof(n);
}
}

namespace StrUtil
{
/*TODO: Add strok() and other string processing utilities here. They are nice to have.*/
}

}

using namespace CMem;

}

#endif
