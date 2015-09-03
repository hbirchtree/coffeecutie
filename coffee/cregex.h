#ifndef CREGEX_H
#define CREGEX_H

#include "coffee.h"
#include <regex>

namespace Coffee{
namespace CFunctional{

struct CRegexMatch
{
    std::vector<std::string> s_match;
    bool b_match;
};

static std::vector<CRegexMatch> coffee_regex_match_multi(const std::string& pattern,
                   const std::vector<std::string>& data,
                   bool capture = false)
{
    std::regex rgx(pattern);
    std::smatch mch;

    std::vector<CRegexMatch> matches;

    for(const std::string& string : data){
        CRegexMatch m;
        m.b_match = false;
        if(std::regex_match(string,mch,rgx)){
            for(size_t i=0;i<mch.size();i++){
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
static CRegexMatch coffee_regex_match(std::string pattern, const std::string& data, bool capture = false)
{
    std::regex rgx(pattern);
    std::smatch mch;
    CRegexMatch m;
    m.b_match = false;
    if(std::regex_match(data,mch,rgx)){
        for(size_t i=0;i<mch.size();i++){
            std::ssub_match smch = mch[i];
            m.b_match = true;
            if(capture)
                m.s_match.push_back(smch.str());
        }
    }
    return m;
}

}
}
#endif // CREGEX_H
