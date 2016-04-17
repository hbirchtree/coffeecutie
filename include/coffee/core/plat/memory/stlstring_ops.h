#pragma once


#include "../plat_quirks_toggling.h"

#include <algorithm>
#include <cctype>

#ifdef COFFEE_USE_IOSTREAMS
#include <iomanip>
#endif

#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Mem{

namespace Search
{
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
}

namespace Convert{
/*
 * In the below functions we remove the null-terminator from the strings.
 * It causes a literal NULL to appear in the strings, which is bad, m'kay?
 *
 */

/* Floating-point conversion */
FORCEDINLINE CString scalarltostring(lscalar const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%Lf",s));
    snprintf(&str[0],str.size(),"%Lf",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString scalartostring(bigscalar const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%f",s));
    snprintf(&str[0],str.size(),"%f",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString scalarftostring(scalar const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%f",s));
    snprintf(&str[0],str.size(),"%f",s);
    str.resize(str.size()-1);
    return str;
}
/* Unsigned integer conversion */
FORCEDINLINE CString uintltostring(uint64 const& s)
{
    if(s==0)
        return "0";
    // TODO: Find out what the fuck is going on here
    uint64 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,"%lu",ss));
    snprintf(&str[0],str.size(),"%lu",ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString uinttostring(uint32 const& s)
{
    if(s==0)
        return "0";
    uint32 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,"%u",ss));
    snprintf(&str[0],str.size(),"%u",ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString uintstostring(uint16 const& s)
{
    if(s==0)
        return "0";
    uint16 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,"%hu",ss));
    snprintf(&str[0],str.size(),"%hu",ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString uintctostring(uint8 const& s)
{
    if(s==0)
        return "0";
    uint8 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,"%hhu",ss));
    snprintf(&str[0],str.size(),"%hhu",ss);
    str.resize(str.size()-1);
    return str;
}
/* Integer conversion */
FORCEDINLINE CString intltostring(int64 const& s)
{
    if(s==0)
        return "0";
    int64 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,"%ld",ss));
    snprintf(&str[0],str.size(),"%ld",ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString inttostring(int32 const& s)
{
    if(s==0)
        return "0";
    int32 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,"%d",ss));
    snprintf(&str[0],str.size(),"%d",ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString intstostring(int16 const& s)
{
    if(s==0)
        return "0";
    int16 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,"%hd",ss));
    snprintf(&str[0],str.size(),"%hd",ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString intctostring(int8 const& s)
{
    if(s==0)
        return "0";
    int8 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,"%hhd",ss));
    snprintf(&str[0],str.size(),"%hhd",ss);
    str.resize(str.size()-1);
    return str;
}

FORCEDINLINE cstring booltostring(bool i)
{
    return (i) ? "true" : "false";
}

}


namespace StrUtil{

FORCEDINLINE CString hexdump(c_cptr ptr, szptr len, bool spacing = true, szptr newline_freq = 0)
{
    byte_t const* data = (byte_t const*)ptr;
    CString out;
    out.reserve(len*2+len-1);

    for(szptr i=0;i<len;i++)
    {
        out.append("  ");
        sprintf(&out[i*2+i],"%02x",data[i]);
        if(newline_freq == 0 || (i+1)%newline_freq != 0)
        {
            if(spacing)
                out.append(" ");
        }else
            out.append("\n");
    }

    return out;
}

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

FORCEDINLINE CString hexify(uint64 inp, bool trim_zero = false)
{
    CString out;
    out.resize(16);

    uint8 a;
    uint64 b;
    for(uint8 i=0;i<64;i+=4)
    {
        a = i / 4;
        b = inp;
        b = b << (64 - ( a + 1) * 4);
        b = b >> 60;
        if(b<10)
            out[15-a] = '0'+b;
        else
            out[15-a] = 'a'+b-10;
    }
    zeroltrim(out);
    if(trim_zero)
        zerortrim(out);
    return out;
}

FORCEDINLINE CString pointerify(uint64 const& ptr)
{
    CString str = "0x";
    str += hexify(ptr);
    return str;
}
FORCEDINLINE CString pointerify(const void* const& ptr)
{
    return pointerify((uint64 const&)ptr);
}

FORCEDINLINE CString lower(CString const& st)
{
    CString o;
    o.reserve(st.size());
    for(CString::value_type c : st)
        o.push_back(std::tolower(c));
    return o;
}

FORCEDINLINE CString upper(CString const& st)
{
    CString o;
    o.reserve(st.size());
    for(CString::value_type c : st)
        o.push_back(std::toupper(c));
    return o;
}

}


}
}
