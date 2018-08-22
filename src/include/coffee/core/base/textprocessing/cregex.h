#pragma once

#include <regex>

#include "../../coffee_mem_macros.h"
#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee {
namespace Regex {

struct regex_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int) const;
};

using regex_error_code =
    domain_error_code<std::regex_constants::error_type, regex_error_category>;

template<bool Capture = true>
struct regex_opts
{
    static constexpr bool capture = Capture;
};

template<typename CharType>
struct regex_types
{
    using string_type = std::basic_string<CharType>;
    using regex_type  = std::basic_regex<CharType>;
};

template<typename RegexTraits = regex_types<char>>
typename RegexTraits::regex_type compile_pattern(
    typename RegexTraits::string_type const& pattern, regex_error_code& ec)
{
    try
    {
        return typename RegexTraits::regex_type(pattern);
    } catch(std::regex_error const& e)
    {
        ec = e.what();
        ec = e.code();
        return {};
    }
}

template<typename RegexTraits = regex_types<char>>
typename RegexTraits::regex_type compile_pattern(
    typename RegexTraits::string_type const& pattern)
{
    regex_error_code ec;
    return compile_pattern(pattern, ec);
}

template<
    typename RegexTraits                               = regex_types<char>,
    typename RegexOpts                                 = regex_opts<>,
    typename std::enable_if<RegexOpts::capture>::type* = nullptr>
bool match(
    typename RegexTraits::regex_type const&    pattern,
    typename RegexTraits::string_type const&   input,
    Vector<typename RegexTraits::string_type>& output)
{
    std::match_results<typename RegexTraits::string_type::const_iterator> match;

    if(!std::regex_match(input, match, pattern))
        return false;

    for(auto const& sub_match : match)
        output.push_back(sub_match.str());

    return true;
}

template<
    typename RegexTraits                                = regex_types<char>,
    typename RegexOpts                                  = regex_opts<false>,
    typename std::enable_if<!RegexOpts::capture>::type* = nullptr>
bool match(
    typename RegexTraits::regex_type const&  pattern,
    typename RegexTraits::string_type const& input)
{
    std::match_results<typename RegexTraits::string_type::iterator> match;

    if(!std::regex_match(input, match, pattern))
        return false;

    return true;
}

using Pattern = regex_types<char>::regex_type;

} // namespace Regex
} // namespace Coffee

namespace Coffee {

// struct RegexDef
//{
//    struct RegMatch;
//    class Pattern;

//    static Pattern Compile(const CString& patt);

//    static Vector<RegMatch> Match(
//        const Pattern&         pattern,
//        const Vector<CString>& data,
//        bool                   capture = false);

//    static Vector<RegMatch> Match(
//        const Pattern& pattern, const CString& data, bool capture = false);
//};

// struct StdRegexImpl : RegexDef
//{
//    struct RegMatch
//    {
//        Vector<CString> s_match; /*!< Matching strings*/
//        bool            b_match; /*!< True if match was found*/

//        u8 padding[7];
//    };

//    using Pattern = std::regex;

//    STATICINLINE Pattern Compile(const CString& patt)
//    {
//        /* Because GCC 4.8.x sucks, std::regex is really unstable */
//        try
//        {
//            return Pattern(patt);
//        } catch(std::regex_error const&)
//        {
//            return {};
//        }
//    }

//    static Vector<RegMatch> Match(
//        const Pattern& rgx, const Vector<CString>& data, bool capture =
//        false);

//    static Vector<RegMatch> Match(
//        const Pattern& rgx, const CString& data, bool capture = false);
//};

// using Regex = RegexImplementation::StdRegexImpl;

} // namespace Coffee
