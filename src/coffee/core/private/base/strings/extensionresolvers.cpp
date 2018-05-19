#include <coffee/core/base/strings/extensionresolvers.h>
#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/CRegex>
#include <coffee/core/plat/plat_memory.h>

#include <type_traits>

namespace Coffee{
namespace Strings{

using namespace Mem;
using namespace Mem::Search;

CString extArgReplacePhrase(const CString &fmt, const CString &phrase, const CString &replace)
{
    return CStrReplace(fmt,phrase,replace);
}

CString extArgReplace(const CString &fmt, const size_t &index, const CString &replace)
{
    CString subfmt = "{" + cast_pod(index) + "}";
    return CStrReplace(fmt,subfmt,replace);
}

template<typename T,

         typename std::enable_if<
             std::is_floating_point<T>::value, bool>::type*
         = nullptr

         >
CString cStringReplace(
        CString const& fmt, size_t const& index,
        T const& arg)
{
    /* Regexes, man, these fucking regexes */
    Regex::Pattern patt = Regex::Compile(".*?(\\{\\d+:(\\d+)\\}).*");
    auto match = Regex::Match(patt,fmt,true);

    if(match.size()>=3)
    {
        uint32 prec = Convert::strtouint(match[2].s_match[0].c_str());

        CString rep;
#ifdef COFFEE_USE_IOSTREAMS
        std::stringstream ss;
        ss << std::fixed << std::setprecision(prec) << arg;
        ss >> rep;
#else
        rep = Convert::scalartostring(arg);
        auto dot1 = rep.find('.');
        auto dot2 = rep.find(',');
        if(dot1)
            rep.resize(dot1+1+prec,'0');
        else if(dot2)
            rep.resize(dot2+1+prec,'0');
#endif

        return extArgReplacePhrase(fmt,match[1].s_match[0],rep);
    }
    else
        return extArgReplace(fmt,index,Strings::to_string(arg));
}

}
}
