#pragma once

#include <tuple>

namespace stl_types::tuple {
namespace detail {

template<std::size_t I, typename... T>
requires(sizeof...(T) == I)
    //
    inline void tuple_for_each(std::tuple<T...>&, auto&&)
{
}

template<std::size_t I, typename... T>
requires(sizeof...(T) > I)
    //
    inline void tuple_for_each(std::tuple<T...>& src, auto&& predicate)
{
    predicate(std::get<I>(src));
    tuple_for_each<I + 1>(src, std::move(predicate));
}

} // namespace detail

template<typename... T>
requires(sizeof...(T) > 0)
    //
    inline void for_each(std::tuple<T...>& src, auto&& predicate)
{
    detail::tuple_for_each<0, T...>(src, predicate);
}

} // namespace stl_types::tuple
