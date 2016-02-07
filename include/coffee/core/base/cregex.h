#ifndef CREGEX_H
#define CREGEX_H

#include <regex>

#include "../coffee.h"

namespace Coffee{
namespace CFunctional{

/*!
 * \brief Regex matches
 */
struct CRegexMatch
{
    std::vector<CString> s_match; /*!< Matching strings*/
    bool b_match; /*!< True if match was found*/
};

/*!
 * \brief Match pattern in multiple strings
 * \param pattern Pattern to apply
 * \param data Data set
 * \param capture Whether to capture
 * \return Array with same size as data containing results
 */
inline C_FORCE_INLINE std::vector<CRegexMatch> RegexMatchMulti(
        const CString& pattern, const std::vector<CString>& data,
        bool capture = false)
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

/*!
 * \brief Match pattern in single string
 * \param pattern Pattern to apply
 * \param data String to test
 * \param capture Whether to capture
 * \return Struct with results
 */
inline C_FORCE_INLINE std::vector<CRegexMatch> RegexMatch(
        CString pattern, const CString& data,
        bool capture = false)
{
    std::regex rgx(pattern);
    std::smatch mch;
    std::vector<CRegexMatch> m;
    if(std::regex_match(data,mch,rgx)){
        for(szptr i=0;i<mch.size();i++){
            std::ssub_match smch = mch[i];
            CRegexMatch e;
            e.b_match = true;
            if(capture)
                e.s_match.push_back(smch.str());
            m.push_back(e);
        }
    }
    return m;
}

}
}
#endif // CREGEX_H
