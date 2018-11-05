#pragma once

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <peripherals/libc/types.h>
#include <peripherals/stl/standard_exceptions.h>

namespace str {

using namespace libc_types;
using namespace type_safety;

template<bool CaseInsensitive, bool NoIndex, bool Bounded = false>
struct comparison_mode
{
    static constexpr bool ignore_case = CaseInsensitive;
    static constexpr bool no_index    = NoIndex;
    static constexpr bool bounded     = Bounded;
};

/* bool result types */
using comp_default = comparison_mode<false, true>;
using comp_nocase  = comparison_mode<true, true>;
/* indexed result types */
using comp_idx        = comparison_mode<false, false>;
using comp_nocase_idx = comparison_mode<true, false>;
/* bounded size */
using comp_bound     = comparison_mode<false, true, true>;
using comp_idx_bound = comparison_mode<false, false, true>;

template<class CompMode, class TargetMode>
struct comparison_of_type
{
    static constexpr bool value =
        (CompMode::bounded == TargetMode::bounded) &&
        (CompMode::ignore_case == TargetMode::ignore_case);
};

template<bool Reversed, bool Characters>
struct find_mode
{
    static constexpr bool reversed   = Reversed;
    static constexpr bool token_list = Characters;
};

using find_default = find_mode<false, false>;
using find_char    = find_mode<false, false>;
using find_tokens  = find_mode<false, true>;
using find_reverse = find_mode<true, false>;

template<
    typename FindMode,
    typename TargetMode,
    typename std::enable_if<FindMode::reversed == TargetMode::reversed>::type* =
        nullptr,
    typename std::enable_if<
        FindMode::token_list == TargetMode::token_list>::type* = nullptr,
    typename std::enable_if<!(
        FindMode::reversed && FindMode::token_list)>::type*    = nullptr>
struct find_mode_of_type
{
    typedef FindMode type;
};

template<unsigned Base>
struct convert_mode
{
    static constexpr unsigned base = Base;
};

using convert_base_0  = convert_mode<0>;
using convert_base_8  = convert_mode<8>;
using convert_base_10 = convert_mode<10>;
using convert_base_16 = convert_mode<16>;

template<
    typename CharType,
    typename TargetType,
    typename std::enable_if<std::is_same<CharType, TargetType>::value>::type* =
        nullptr,
    typename std::enable_if<
        std::is_same<TargetType, char>::value ||
        std::is_same<TargetType, wchar_t>::value>::type* = nullptr>
struct char_of_type
{
    typedef CharType type;
};

template<
    typename CharType,
    typename char_of_type<CharType, char>::type* = nullptr>
FORCEDINLINE size_t len(const CharType* s)
{
    if(!s)
        return 0;

    return ::strlen(s);
}

template<
    typename CharType,
    typename char_of_type<CharType, wchar_t>::type* = nullptr>
FORCEDINLINE size_t len(const CharType* s)
{
    if(!s)
        return 0;

    return ::wcslen(s);
}

namespace compare_ops {

template<
    typename CompMode,
    typename CharType,
    typename std::enable_if<
        comparison_of_type<CompMode, comp_default>::value>::type* = nullptr,
    typename char_of_type<CharType, char>::type*                  = nullptr>
FORCEDINLINE ptrdiff_t cmp(const CharType* s1, const CharType* s2)
{
    return ::strcmp(s1, s2);
}

template<
    typename CompMode,
    typename CharType,
    typename std::enable_if<
        comparison_of_type<CompMode, comp_default>::value>::type* = nullptr,
    typename char_of_type<CharType, wchar_t>::type*               = nullptr>
FORCEDINLINE ptrdiff_t cmp(const CharType* s1, const CharType* s2)
{
    return ::wcscmp(s1, s2);
}

template<
    typename CompMode,
    typename CharType,
    typename std::enable_if<
        comparison_of_type<CompMode, comp_bound>::value>::type* = nullptr,
    typename char_of_type<CharType, char>::type*                = nullptr>
FORCEDINLINE ptrdiff_t cmp(const CharType* s1, const CharType* s2)
{
    return ::strncmp(s1, s2);
}

template<
    typename CompMode,
    typename CharType,
    typename std::enable_if<
        comparison_of_type<CompMode, comp_bound>::value>::type* = nullptr,
    typename char_of_type<CharType, wchar_t>::type*             = nullptr>
FORCEDINLINE ptrdiff_t cmp(const CharType* s1, const CharType* s2)
{
    return ::wcsncmp(s1, s2);
}

#if defined(COFFEE_UNIXPLAT)
#define C_CASECMP ::strcasecmp
#define C_WCASECMP ::wcscasecmp
#elif defined(COFFEE_WINDOWS_UWP)
#define C_CASECMP ::_stricmp
#define C_WCASECMP ::_wcsicmp
#elif defined(COFFEE_GEKKO)
#define C_CASECMP ::strcasecmp
#define C_WCASECMP ::wcscasecmp
#else
#define C_CASECMP ::stricmp
#define C_WCASECMP ::wcsicmp
#endif

template<
    typename CompMode,
    typename CharType,
    typename std::enable_if<
        comparison_of_type<CompMode, comp_nocase>::value>::type* = nullptr,
    typename char_of_type<CharType, char>::type*                 = nullptr>
FORCEDINLINE ptrdiff_t cmp(const CharType* s1, const CharType* s2)
{
    return C_CASECMP(s1, s2);
}

template<
    typename CompMode,
    typename CharType,
    typename std::enable_if<
        comparison_of_type<CompMode, comp_nocase>::value>::type* = nullptr,
    typename char_of_type<CharType, wchar_t>::type*              = nullptr>
FORCEDINLINE ptrdiff_t cmp(const CharType* s1, const CharType* s2)
{
    return C_WCASECMP(s1, s2);
}

#undef C_CASECMP
#undef C_WCASECMP

} // namespace compare_ops

template<
    typename CompMode                                  = comp_default,
    typename CharType                                  = char,
    typename std::enable_if<CompMode::no_index>::type* = nullptr>
FORCEDINLINE bool cmp(const CharType* s1, const CharType* s2)
{
    return compare_ops::cmp<CompMode, CharType>(s1, s2) == 0;
}
template<
    typename CompMode                                   = comp_default,
    typename CharType                                   = char,
    typename std::enable_if<!CompMode::no_index>::type* = nullptr>
FORCEDINLINE ptrdiff_t cmp(const CharType* s1, const CharType* s2)
{
    return compare_ops::cmp<CompMode, CharType>(s1, s2);
}

namespace find_ops {

template<
    typename FindMode,
    typename CharType,
    typename find_mode_of_type<FindMode, find_default>::type* = nullptr,
    typename char_of_type<CharType, char>::type*              = nullptr>
FORCEDINLINE const CharType* find(
    const CharType* haystack, const CharType* needle)
{
    return ::strstr(haystack, needle);
}

template<
    typename FindMode,
    typename CharType,
    typename find_mode_of_type<FindMode, find_default>::type* = nullptr,
    typename char_of_type<CharType, wchar_t>::type*           = nullptr>
FORCEDINLINE const CharType* find(
    const CharType* haystack, const CharType* needle)
{
    return ::wcsstr(haystack, needle);
}

template<
    typename FindMode,
    typename CharType,
    typename find_mode_of_type<FindMode, find_default>::type* = nullptr,
    typename char_of_type<CharType, char>::type*              = nullptr>
FORCEDINLINE const CharType* find(const CharType* haystack, CharType needle)
{
    return ::strchr(haystack, needle);
}

template<
    typename FindMode,
    typename CharType,
    typename find_mode_of_type<FindMode, find_default>::type* = nullptr,
    typename char_of_type<CharType, wchar_t>::type*           = nullptr>
FORCEDINLINE const CharType* find(const CharType* haystack, CharType needle)
{
    return ::wcschr(haystack, needle);
}

template<
    typename FindMode,
    typename CharType,
    typename find_mode_of_type<FindMode, find_reverse>::type* = nullptr,
    typename char_of_type<CharType, char>::type*              = nullptr>
FORCEDINLINE const CharType* find(const CharType* haystack, CharType needle)
{
    return ::strrchr(haystack, needle);
}

template<
    typename FindMode,
    typename CharType,
    typename find_mode_of_type<FindMode, find_reverse>::type* = nullptr,
    typename char_of_type<CharType, wchar_t>::type*           = nullptr>
FORCEDINLINE const CharType* find(const CharType* haystack, CharType needle)
{
    return ::wcsrchr(haystack, needle);
}

template<
    typename FindMode,
    typename CharType,
    typename find_mode_of_type<FindMode, find_tokens>::type* = nullptr,
    typename char_of_type<CharType, char>::type*             = nullptr>
FORCEDINLINE const CharType* find(
    const CharType* haystack, const CharType* needle)
{
    return ::strpbrk(haystack, needle);
}

template<
    typename FindMode,
    typename CharType,
    typename find_mode_of_type<FindMode, find_tokens>::type* = nullptr,
    typename char_of_type<CharType, wchar_t>::type*          = nullptr>
FORCEDINLINE const CharType* find(
    const CharType* haystack, const CharType* needle)
{
    return ::wcspbrk(haystack, needle);
}

} // namespace find_ops

template<
    typename FindMode                                   = find_default,
    typename CharType                                   = char,
    typename std::enable_if<!FindMode::reversed>::type* = nullptr>
FORCEDINLINE const CharType* find(
    const CharType* haystack, const CharType* needle)
{
    return find_ops::find<FindMode, CharType>(haystack, needle);
}

template<typename FindMode = find_default, typename CharType = char>
FORCEDINLINE const CharType* find(const CharType* haystack, CharType needle)
{
    return find_ops::find<FindMode, CharType>(haystack, needle);
}

namespace convert_ops {

#define STRTOFB_FUNCTION(itype, converter, stype)                           \
    template<                                                               \
        typename CType,                                                     \
        typename std::enable_if<std::is_same<CType, itype>::value>::type* = \
            nullptr>                                                        \
    FORCEDINLINE itype to_float(const stype* n, stype** end_ptr)            \
    {                                                                       \
        return ::converter(n, end_ptr);                                     \
    }

#define STRTOIB_FUNCTION(itype, converter, stype)                          \
    template<                                                              \
        typename CType,                                                    \
        typename ConvertMode,                                              \
        typename std::enable_if<is_similar<CType, itype>::value>::type* =  \
            nullptr>                                                       \
    FORCEDINLINE itype to_int(const stype* n, stype** end_ptr)             \
    {                                                                      \
        static_assert(ConvertMode::base >= 2, "Invalid conversion base");  \
        return C_FCAST<itype>(::converter(n, end_ptr, ConvertMode::base)); \
    }

#define STRTOI_FUNCTION(itype, converter)         \
    STRTOIB_FUNCTION(itype, str##converter, char) \
    STRTOIB_FUNCTION(itype, wcs##converter, wchar_t)

#define STRTOI_ALL()           \
    STRTOI_FUNCTION(i8, tol)   \
    STRTOI_FUNCTION(i16, tol)  \
    STRTOI_FUNCTION(i32, tol)  \
    STRTOI_FUNCTION(i64, toll) \
                               \
    STRTOI_FUNCTION(u8, toul)  \
    STRTOI_FUNCTION(u16, toul) \
    STRTOI_FUNCTION(u32, toul) \
    STRTOI_FUNCTION(u64, toull)

STRTOI_ALL()

#define STRTOF_FUNCTION(itype, converter)         \
    STRTOFB_FUNCTION(itype, str##converter, char) \
    STRTOFB_FUNCTION(itype, wcs##converter, wchar_t)

#define STRTOF_ALL()                \
    STRTOF_FUNCTION(scalar, tof)    \
    STRTOF_FUNCTION(bigscalar, tod) \
    STRTOF_FUNCTION(lscalar, told)

STRTOF_ALL()

#undef STRTOFB_FUNCTION
#undef STRTOIB_FUNCTION
#undef STRTOI_BASES_FUNCTION
#undef STRTOI_FUNCTION
#undef STRTOI_ALL
#undef STRTOF_FUNCTION
#undef STRTOF_ALL
}

template<
    typename PodType,
    typename ConvertMode = convert_base_10,
    typename CharType    = char,
    typename std::enable_if<
        std::is_integral<PodType>::value &&
        !std::is_same<PodType, bool>::value>::type* = nullptr>
FORCEDINLINE PodType from_string(const CharType* s)
{
    CharType* end_ptr = nullptr;

    auto out = convert_ops::to_int<PodType, ConvertMode>(s, &end_ptr);

    if(end_ptr < s + len(s))
        Throw(std::invalid_argument("not a number"));

    return out;
}

template<
    typename PodType,
    typename ConvertMode = convert_base_0,
    typename CharType    = char,
    typename std::enable_if<std::is_floating_point<PodType>::value>::type* =
        nullptr>
FORCEDINLINE PodType from_string(const CharType* s)
{
    CharType* end_ptr = nullptr;

    auto out = convert_ops::to_float<PodType>(s, &end_ptr);

    if(end_ptr < s + len(s))
        Throw(std::invalid_argument("not a number"));

    return out;
}

template<
    typename PodType,
    typename ConvertMode                         = convert_base_0,
    typename CharType                            = char,
    typename char_of_type<CharType, char>::type* = nullptr,
    typename std::enable_if<std::is_same<PodType, bool>::value>::type* =
        nullptr>
FORCEDINLINE PodType from_string(const CharType* s)
{
    if(cmp<comp_nocase>(s, "true"))
        return true;
    else if(cmp<comp_nocase>(s, "false"))
        return false;

    Throw(std::invalid_argument("not a boolean string"));
}

template<
    typename PodType,
    typename ConvertMode                            = convert_base_0,
    typename CharType                               = wchar_t,
    typename char_of_type<CharType, wchar_t>::type* = nullptr,
    typename std::enable_if<std::is_same<PodType, bool>::value>::type* =
        nullptr>
FORCEDINLINE PodType from_string(const CharType* s)
{
    if(cmp<comp_nocase>(s, L"true"))
        return true;
    else if(cmp<comp_nocase>(s, L"false"))
        return false;

    Throw(std::invalid_argument("not a boolean string"));
}

} // namespace str
