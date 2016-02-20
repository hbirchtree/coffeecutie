#ifndef CREGEX_H
#define CREGEX_H

#include <regex>

#include "../coffee_mem_macros.h"
#include "../types/tdef/stltypes.h"
#include "../types/tdef/integertypes.h"

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
    };

    using Pattern = std::regex;

    STATICINLINE Pattern Compile(const CString& patt)
    {
        return Pattern(patt);
    }

    STATICINLINE Vector<RegMatch> Match(
            const Pattern& rgx,
            const Vector<CString>& data,
            bool capture = false)
    {
        std::smatch mch;

        Vector<RegMatch> matches;

        for(const CString& string : data){
            RegMatch m;
            m.b_match = false;
            if(std::regex_match(string,mch,rgx)){
                for(szptr i=0;i<mch.size();i++){
                    std::ssub_match smch = mch[i];
                    m.b_match = true;
                    if(capture)
                        m.s_match.push_back(smch.str());
                }
            }
            matches.push_back(m);
        }
        return matches;
    }

    STATICINLINE Vector<RegMatch> Match(
            const Pattern& rgx,
            const CString& data,
            bool capture = false)
    {
        std::smatch mch;
        Vector<RegMatch> matches;
        if(std::regex_match(data,mch,rgx)){
            for(szptr i=0;i<mch.size();i++){
                std::ssub_match smch = mch[i];
                RegMatch e;
                e.b_match = true;
                if(capture)
                    e.s_match.push_back(smch.str());
                matches.push_back(e);
            }
        }
        return matches;
    }
};

}

using Regex = RegexImplementation::StdRegexImpl;

}
#endif // CREGEX_H
