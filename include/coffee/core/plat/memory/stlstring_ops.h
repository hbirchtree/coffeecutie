#pragma once

#include <cctype>
#include <algorithm>
#include <iomanip>

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

namespace StrUtil{

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

template<typename T>
FORCEDINLINE CString spacepad(T const& s, uint32 padding = 1)
{
    CString out;
    std::stringstream ss;
    ss << std::setw(padding) << s;
    ss >> out;
    return out;
}

FORCEDINLINE CString hexify(uint64 i)
{
    CString out;
    std::stringstream ss;
    ss << std::hex << i;
    ss >> out;
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
    std::locale l;
    CString o;
    o.reserve(st.size());
    for(CString::value_type c : st)
	o.push_back(std::tolower(c,l));
    return o;
}

FORCEDINLINE CString upper(CString const& st)
{
    std::locale l;
    CString o;
    o.reserve(st.size());
    for(CString::value_type c : st)
	o.push_back(std::toupper(c,l));
    return o;
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
FORCEDINLINE CString uinttostring(uint64 const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%lu",s));
    snprintf(&str[0],str.size(),"%lu",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString uinttostring(uint32 const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%u",s));
    snprintf(&str[0],str.size(),"%u",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString uinttostring(uint16 const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%hu",s));
    snprintf(&str[0],str.size(),"%hu",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString uinttostring(uint8 const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%hhu",s));
    snprintf(&str[0],str.size(),"%hhu",s);
    str.resize(str.size()-1);
    return str;
}
/* Integer conversion */
FORCEDINLINE CString inttostring(int64 const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%li",s));
    snprintf(&str[0],str.size(),"%li",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString inttostring(int32 const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%i",s));
    snprintf(&str[0],str.size(),"%i",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString inttostring(int16 const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%hi",s));
    snprintf(&str[0],str.size(),"%hi",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString inttostring(int8 const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%hhi",s));
    snprintf(&str[0],str.size(),"%hhi",s);
    str.resize(str.size()-1);
    return str;
}

FORCEDINLINE cstring booltostring(bool i)
{
    return (i) ? "true" : "false";
}

}
}
}
