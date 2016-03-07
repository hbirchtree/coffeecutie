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

}
}
}
