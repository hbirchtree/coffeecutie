#pragma once

#include <regex>

#include <peripherals/error/regex.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace stl_types {
namespace regex {

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
    std::vector<typename RegexTraits::string_type>& output)
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

} // namespace regex
} // namespace stl_types
