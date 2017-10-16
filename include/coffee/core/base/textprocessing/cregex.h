#ifndef CREGEX_H
#define CREGEX_H

#if !defined(COFFEE_ANDROID) || 1
#include <regex>
#endif

#include "../../coffee_mem_macros.h"
#include "../../types/tdef/stltypes.h"
#include "../../types/tdef/integertypes.h"

namespace Coffee{
namespace RegexImplementation
{
struct RegexDef
{
    struct RegMatch;
    class Pattern;

    static Pattern Compile(const CString& patt);

    static Vector<RegMatch> Match(
            const Pattern& pattern,
            const Vector<CString>& data,
            bool capture = false);

    static Vector<RegMatch> Match(
            const Pattern& pattern,
            const CString& data,
            bool capture = false);
};

struct StdRegexImpl : RegexDef
{
    struct RegMatch
    {
        Vector<CString> s_match; /*!< Matching strings*/
        bool b_match; /*!< True if match was found*/

        uint8 padding[7];
    };

#if !defined(COFFEE_ANDROID) || 1
    using Pattern = std::regex;
#else
    using Pattern = std::string;
#endif

    STATICINLINE Pattern Compile(const CString& patt)
    {
        return Pattern(patt);
    }

    static Vector<RegMatch> Match(
            const Pattern& rgx,
            const Vector<CString>& data,
            bool capture = false);

    static Vector<RegMatch> Match(
            const Pattern& rgx,
            const CString& data,
            bool capture = false);
};

}

using Regex = RegexImplementation::StdRegexImpl;

}
#endif // CREGEX_H
