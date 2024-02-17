#pragma once

#include "decl_member_function.h"
#include "types.h"

#include <functional>
#include <peripherals/libc/types.h>

namespace stl_types {

template<typename FunSignature>
using Function = std::function<FunSignature>;

template<typename T>
struct WkPtrUnwrap
{
    WkPtrUnwrap(std::weak_ptr<T>&& ptr)
    {
        lock = ptr.lock();
    }

    WkPtrUnwrap(std::weak_ptr<T>& ptr)
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

    std::shared_ptr<T> lock;
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
    Ret (ClassName::*func_ref)(std::tuple<IncludedArgs...>, Args...),
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
    Ret (ClassName::*func_ref)(std::tuple<IncludedArgs...>, Args...),
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
inline void wrap_exception(Function<void(EType const&)> exHandler, Args... args)
{
    try
    {
        std::invoke(std::forward<Args>(args)...);
    } catch(EType const& e)
    {
        exHandler(e);
    }
}

} // namespace stl_types
