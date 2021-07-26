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

template<typename FunSignature>
using Function = std::function<FunSignature>;

template<typename T>
struct WkPtrUnwrap
{
    WkPtrUnwrap(WkPtr<T>&& ptr)
    {
        lock = ptr.lock();
    }
    WkPtrUnwrap(WkPtr<T>& ptr)
    {
        lock = ptr.lock();
    }

    void operator()(Function<void(T*)>&& present)
    {
        if(!lock)
            return;

        present(lock.get());
    }

    operator bool() const
    {
        return static_cast<bool>(lock);
    }

    ShPtr<T> lock;
};

template<typename PtrType>
FORCEDINLINE void ApplyIfValid(
    PtrType&& ptr, Function<void(typename PtrType::element_type*)>&& func)
{
    WkPtrUnwrap<typename PtrType::element_type> wrapper(std::move(ptr));
    wrapper(std::move(func));
}

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

template<typename EType, typename... Args>
inline void wrap_exception(
    Function<void(EType const&)> exHandler,
    Args... args)
{
    try
    {
        std::invoke(std::forward<Args>(args)...);
    } catch(EType const& e)
    {
        exHandler(e);
    }
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

} // namespace stl_types

#if __cplusplus >= 201703L
#define declmemtype(fun) decltype(mem_function_traits(fun))::result_type
#define declmemtype2(T, fun) \
    decltype(std::apply(&T::fun, std::declval<mem_args_tuple<T>>()))
#else
#define declmemtype(fun) decltype(std::mem_fn(fun))::result_type
#endif

#define declreturntype(fun) \
    decltype(stl_types::function_traits(&fun))::result_type
