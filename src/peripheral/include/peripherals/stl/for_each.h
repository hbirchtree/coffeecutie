#pragma once

#include <functional>

namespace stl_types {

template<class Container>
inline void for_each(
    Container const&                                                  c,
    std::function<void(typename Container::value_type const&)> const& predicate)
{
    for(auto const& v : c)
        predicate(v);
}

namespace detail {

template<typename Type, typename Other>
requires std::is_same_v<Type, Other>
//
inline void predicate_if_type(
    std::function<void(Type&)> const& predicate, Other& value)
{
    predicate(std::ref(value));
}

template<typename Type, typename Other>
requires(!std::is_same_v<Type, Other>)
//
inline void predicate_if_type(std::function<void(Type&)> const&, Other&)
{
    // NO-OP
}

} // namespace detail

template<typename Type, typename... Args>
inline void for_each_if_type(
    std::function<void(Type&)> const& predicate, Args&... args)
{
    (detail::predicate_if_type<Type>(predicate, args), ...);
}

} // namespace stl_types
