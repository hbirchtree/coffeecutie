#include <coffee/core/CRegex>
#include <coffee/core/base/strings/extensionresolvers.h>
#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/plat/plat_memory.h>

#include <type_traits>

namespace Coffee {
namespace Strings {

using namespace Mem;

CString extArgReplacePhrase(
    const CString& fmt, const CString& phrase, const CString& replace)
{
    return str::replace::str(fmt, phrase, replace);
}

CString extArgReplace(
    const CString& fmt, const size_t& index, const CString& replace)
{
    // TODO: This needs optimizations
    CString subfmt = "{" + cast_pod(index) + "}";
    return str::replace::str(fmt, subfmt, replace);
}

template<
    typename T,

    typename std::enable_if<std::is_floating_point<T>::value, bool>::type* =
        nullptr

    >
CString cStringReplace(CString const& fmt, size_t const& index, T const& arg)
{
    /* Regexes, man, these fucking regexes */
    Regex::Pattern  patt = Regex::compile_pattern(".*?(\\{\\d+:(\\d+)\\}).*");
    Vector<CString> match;

    if(!Regex::match(patt, fmt, match))
        return extArgReplace(fmt, index, Strings::to_string(arg));

    u32 prec = str::from_string<u32>(match[2].c_str());

    CString rep;
#ifdef COFFEE_USE_IOSTREAMS
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << arg;
    ss >> rep;
#else
    rep       = str::convert::to_string(arg);
    auto dot1 = rep.find('.');
    auto dot2 = rep.find(',');
    if(dot1)
        rep.resize(dot1 + 1 + prec, '0');
    else if(dot2)
        rep.resize(dot2 + 1 + prec, '0');
#endif

    return extArgReplacePhrase(fmt, match[1], rep);
}

} // namespace Strings
} // namespace Coffee
