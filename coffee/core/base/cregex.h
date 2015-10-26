#ifndef CREGEX_H
#define CREGEX_H

#include "coffee/core/coffee.h"
#include <regex>

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
extern std::vector<CRegexMatch> coffee_regex_match_multi(
        const CString& pattern, const std::vector<CString>& data,
        bool capture = false);

/*!
 * \brief Match pattern in single string
 * \param pattern Pattern to apply
 * \param data String to test
 * \param capture Whether to capture
 * \return Struct with results
 */
extern CRegexMatch coffee_regex_match(
        CString pattern, const CString& data,
        bool capture = false);

}
}
#endif // CREGEX_H
