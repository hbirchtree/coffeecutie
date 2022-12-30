#pragma once

#include <functional>
#include <tuple>

namespace stl_types {

/* Source: https://stackoverflow.com/a/19193825 */

template<typename T>
struct function_traits_impl;

template<typename R, typename... Args>
struct function_traits_impl<std::function<R(Args...)>>
{
    static const std::size_t nargs = sizeof...(Args);

    using result_type     = R;
    using arguments_tuple = std::tuple<std::decay_t<Args>...>;

    template<std::size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template<class R, class T, class... Args>
constexpr function_traits_impl<std::function<R(Args...)>> function_traits(
    R (T::*)(Args...) const)
{
    return function_traits_impl<std::function<R(Args...)>>();
}

template<class R, class T, class... Args>
constexpr function_traits_impl<std::function<R(Args...)>> function_traits(
    R (T::*)(Args...))
{
    return function_traits_impl<std::function<R(Args...)>>();
}

template<class R, class... Args>
constexpr function_traits_impl<std::function<R(Args...)>> function_traits(
    R (*)(Args...))
{
    return function_traits_impl<std::function<R(Args...)>>();
}

template<typename T, typename... Args>
using mem_args_tuple = std::tuple<T*, Args...>;

template<class T, typename R, typename... Args>
struct mem_function_traits
{
    using class_type  = T;
    using result_type = R;
    using args_tuple  = mem_args_tuple<T, Args...>;

    using signature = R(Args...);

    mem_function_traits(R (T::*)(Args...) const)
    {
    }
    mem_function_traits(R (T::*)(Args...))
    {
    }
};

template<class T, typename V>
struct member_traits
{
    constexpr member_traits(V T::*member) : member(member)
    {
    }

    using class_type = T;
    using value_type = V;

    static constexpr size_t class_size = sizeof(T);
    static constexpr size_t value_size = sizeof(V);

    auto offset() const
    {
        constexpr T* base = nullptr;
        return reinterpret_cast<uintptr_t>(&(base->*member));
    }

    V T::*member;
};

} // namespace stl_types

#if __cplusplus >= 201703L
#define declmemtype(fun) decltype(stl_types::mem_function_traits(fun))::result_type
#define declmemtype2(T, fun) \
    decltype(std::apply(&T::fun, std::declval<mem_args_tuple<T>>()))
#else
#define declmemtype(fun) decltype(std::mem_fn(fun))::result_type
#endif

#define declreturntype(fun) \
    decltype(stl_types::function_traits(&fun))::result_type
