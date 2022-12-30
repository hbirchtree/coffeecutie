#pragma once

#include "proxy.h"

namespace compo {
namespace detail {

template<class T>
struct single_selector
{
    template<class Container>
    STATICINLINE bool match(Container& c, u64 id)
    {
        return c.template get<T>(id) != nullptr;
    }
};

struct false_selector
{
    using type = false_selector;

    template<class Container>
    STATICINLINE bool match(Container&, u64)
    {
        return false;
    }
};

struct true_selector
{
    using type = true_selector;

    template<class Container>
    STATICINLINE bool match(Container&, u64)
    {
        return true;
    }
};

template<typename... T>
struct lazy_any;

template<class T, class... Types>
struct any_selector
{
    template<class Container>
    STATICINLINE bool match(Container& c, u64 id)
    {
        return single_selector<T>::match(c, id) ||
               std::conditional<
                   sizeof...(Types) >= 1,
                   lazy_any<Types...>,
                   false_selector>::type::type::match(c, id);
    }
};

template<typename... T>
struct lazy_any
{
    using type = any_selector<T...>;
};

template<typename... T>
struct lazy_all;

template<class T, class... Types>
struct all_selector
{
    template<class Container>
    STATICINLINE bool match(Container& c, u64 id)
    {
        return single_selector<T>::match(c, id) &&
                std::conditional<sizeof...(Types) >= 1,
                lazy_all<Types...>,
                true_selector>::type::type::match(c, id);
    }
};

template<typename... T>
struct lazy_all
{
    using type = all_selector<T...>;
};

} // namespace detail

namespace selector {

template<class T, class... Types>
using any = detail::any_selector<T, Types...>;

template<class T, class... Types>
using all = detail::all_selector<T, Types...>;

template<class T>
using only = detail::single_selector<T>;

}

} // namespace compo
