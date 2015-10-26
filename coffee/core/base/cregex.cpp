#include "cregex.h"

namespace Coffee{
namespace CFunctional{
std::vector<CRegexMatch> coffee_regex_match_multi(
        const CString &pattern, const std::vector<CString> &data,
        bool capture)
{
    std::regex rgx(pattern);
    std::smatch mch;

    std::vector<CRegexMatch> matches;

    for(const CString& string : data){
        CRegexMatch m;
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

CRegexMatch coffee_regex_match(
        CString pattern, const CString &data, bool capture)
{
    std::regex rgx(pattern);
    std::smatch mch;
    CRegexMatch m;
    m.b_match = false;
    if(std::regex_match(data,mch,rgx)){
        for(szptr i=0;i<mch.size();i++){
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
