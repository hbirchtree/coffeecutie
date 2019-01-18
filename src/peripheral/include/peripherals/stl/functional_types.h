#pragma once

#include "types.h"

#include <functional>
#include <peripherals/libc/types.h>

namespace stl_types {

/* Source: https://stackoverflow.com/a/19193825 */

template<typename T>
struct function_traits_impl;

template<typename R, typename... Args>
struct function_traits_impl<std::function<R(Args...)>>
{
    static const std::size_t nargs = sizeof...(Args);

    typedef R result_type;

    template<std::size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template<class R, class T, class... Args>
function_traits_impl<std::function<R(Args...)>> function_traits(
    R (T::*)(Args...) const)
{
    return function_traits_impl<std::function<R(Args...)>>();
}

template<class R, class T, class... Args>
function_traits_impl<std::function<R(Args...)>> function_traits(
    R (T::*)(Args...))
{
    return function_traits_impl<std::function<R(Args...)>>();
}

template<class R, class... Args>
function_traits_impl<std::function<R(Args...)>> function_traits(R (*)(Args...))
{
    return function_traits_impl<std::function<R(Args...)>>();
}

} // namespace stl_types

#define declmemtype(fun) decltype(std::mem_fun(&fun))::result_type
#define declreturntype(fun) \
    decltype(stl_types::function_traits(&fun))::result_type

namespace stl_types {

template<typename FunSignature>
using Function = std::function<FunSignature>;

namespace bind_this {
template<
    typename ClassName,
    typename Ret,
    typename... Args,
    typename... IncludedArgs,
    typename std::enable_if<!std::is_same<Ret, void>::value>::type* = nullptr>
Function<Ret(Args...)> func(
    ClassName* this_ref,
    Ret (ClassName::*func_ref)(std::tuple<IncludedArgs...>, Args...),
    IncludedArgs... iargs)
{
    return [=](Args... args) {
        return (*this_ref.*func_ref)(std::make_tuple(iargs...), args...);
    };
}

template<
    typename ClassName,
    typename Ret,
    typename... Args,
    typename... IncludedArgs,
    typename std::enable_if<std::is_same<Ret, void>::value>::type* = nullptr>
Function<void(Args...)> func(
    ClassName* this_ref,
    Ret (ClassName::*func_ref)(Tup<IncludedArgs...>, Args...),
    IncludedArgs... iargs)
{
    return [=](Args... args) {
        (*this_ref.*func_ref)(std::make_tuple(iargs...), args...);
    };
}

template<
    typename ClassName,
    typename Ret,
    typename... Args,
    typename... IncludedArgs,
    typename std::enable_if<!std::is_same<Ret, void>::value>::type* = nullptr>
Function<Ret(Args...)> func(
    ClassName* this_ref,
    Ret (ClassName::*func_ref)(Tup<IncludedArgs...>, Args...),
    std::tuple<IncludedArgs...> iargs)
{
    return [=](Args... args) { return (*this_ref.*func_ref)(iargs, args...); };
}

template<
    typename ClassName,
    typename Ret,
    typename... Args,
    typename... IncludedArgs,
    typename std::enable_if<std::is_same<Ret, void>::value>::type* = nullptr>
Function<void(Args...)> func(
    ClassName* this_ref,
    Ret (ClassName::*func_ref)(std::tuple<IncludedArgs...>, Args...),
    std::tuple<IncludedArgs...> iargs)
{
    return [=](Args... args) { (*this_ref.*func_ref)(iargs, args...); };
}
} // namespace bind_this

namespace quiet_exception {

template<
    typename ExceptType,
    typename RType,
    typename... Args,
    typename std::enable_if<!std::is_same<RType, void>::value>::type* = nullptr>
RType call(RType (*fun)(Args...), bool& triggered, Args... args)
{
    try
    {
        RType v   = fun(args...);
        triggered = false;
        return v;
    } catch(ExceptType const&)
    {
        triggered = true;
        return RType();
    }
}

template<
    typename ExceptType,
    typename RType,
    typename... Args,
    typename std::enable_if<std::is_same<RType, void>::value>::type* = nullptr>
void call(RType (*fun)(Args...), Args... args)
{
    try
    {
        fun(args...);
    } catch(ExceptType const&)
    {
    }
}

} // namespace quiet_exception

} // namespace stl_types
