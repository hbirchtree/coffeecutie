#include <coffee/core/base/textprocessing/cregex.h>

namespace Coffee{
namespace RegexImplementation{

Vector<StdRegexImpl::RegMatch> StdRegexImpl::Match(const StdRegexImpl::Pattern &rgx, const Vector<CString> &data, bool capture)
{
#if !defined(COFFEE_ANDROID)
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
#else
    return {};
#endif
}

Vector<StdRegexImpl::RegMatch> StdRegexImpl::Match(const StdRegexImpl::Pattern &rgx, const CString &data, bool capture)
{
#if !defined(COFFEE_ANDROID)
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
#else
    return {};
#endif
}



}
}
