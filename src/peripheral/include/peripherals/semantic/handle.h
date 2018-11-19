#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace semantic {

using namespace stl_types;

template<
    typename hnd_type,
    bool     enable_exception    = true,
    hnd_type InvalidValue        = hnd_type(),
    void (*close_func)(hnd_type) = nullptr,

    typename std::enable_if<
        std::is_pod<hnd_type>::value ||
        std::is_pointer<hnd_type>::value>::type* = nullptr

    >
struct generic_handle_t : non_copy
{
    generic_handle_t() : hnd(hnd_type())
    {
    }

    generic_handle_t(generic_handle_t&& other) : hnd(other.hnd)
    {
        other.hnd = InvalidValue;
    }

    explicit generic_handle_t(hnd_type handle) : hnd(handle)
    {
    }

    void handle_check()
    {
#if MODE_DEBUG
        if(hnd != InvalidValue)
            Throw(resource_leak("resource leakage detected"));
#endif
    }

    template<
        typename Dummy                                          = int,
        typename std::enable_if<enable_exception, Dummy>::type* = nullptr>
    void handle_check_enable()
    {
        static_assert(
            enable_exception && close_func,
            "close function will never be called");

        handle_check();
    }
    template<
        typename Dummy                                           = int,
        typename std::enable_if<!enable_exception, Dummy>::type* = nullptr>
    void handle_check_enable()
    {
        if(close_func && hnd != InvalidValue)
            close_func(hnd);
    }

    ~generic_handle_t()
    {
        handle_check_enable();
    }

    C_DELETE_COPY_CONSTRUCTOR(generic_handle_t);

    bool operator!() const
    {
        return hnd == InvalidValue;
    }

    explicit operator hnd_type() const
    {
        return hnd;
    }

    operator bool() const
    {
        return hnd != InvalidValue;
    }

    void release()
    {
        hnd = InvalidValue;
    }

    generic_handle_t<hnd_type>& operator=(hnd_type otherHandle)
    {
        handle_check();
        hnd = otherHandle;
        return *this;
    }

    generic_handle_t<hnd_type>& operator=(
        generic_handle_t<hnd_type>&& otherHandle)
    {
        handle_check();

        hnd = otherHandle.hnd;
        otherHandle.release();
        return *this;
    }

    hnd_type hnd;
};

} // namespace semantic
