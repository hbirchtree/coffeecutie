#pragma once

#include <functional>
#include <type_traits>

namespace stl_types {

template<typename Predicate, typename... Options>
constexpr bool is_any_of_v =
    (std::is_same_v<std::decay_t<Predicate>, std::decay_t<Options>> || ...);

template<typename Predicate, typename... Options>
concept is_any_of = is_any_of_v<Predicate, Options...>;

template<typename T, typename... Others>
requires(
    (std::is_same_v<T, Others> && ...) ||
    (std::is_convertible_v<Others, T> && ...))
//
constexpr inline bool any_of(T compare, Others... others)
{
    return ((compare == others) || ...);
}

template<typename T, typename... Others>
requires(
    (std::is_same_v<T, Others> && ...) ||
    (std::is_convertible_v<Others, T> && ...))
//
inline bool any_flag_of(T compare, Others... others)
{
    return (((compare & others) == others) || ...);
}

template<typename T, typename... Others>
requires(std::is_same_v<T, Others> && ...)
inline constexpr bool one_of(T compare, Others... others)
{
    return ((compare == others) + ...) == 1;
}

template<class Container>
inline bool any_of(
    Container const&                                                  c,
    std::function<bool(typename Container::value_type const&)> const& predicate)
{
    size_t sum = 0;
    for(auto const& v : c)
        sum += predicate(v) ? 1 : 0;
    return sum > 0;
}

} // namespace stl_types
