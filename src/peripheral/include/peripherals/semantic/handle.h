#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace semantic {

using namespace stl_types;

template<
    typename hnd_type,
    bool enable_exception = true,

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
        other.hnd = hnd_type();
    }

    explicit generic_handle_t(hnd_type handle) : hnd(handle)
    {
    }

    void handle_check()
    {
#if MODE_DEBUG
        if(hnd != hnd_type())
            Throw(resource_leak("resource leakage detected"));
#endif
    }

    template<
        typename Dummy                                          = int,
        typename std::enable_if<enable_exception, Dummy>::type* = nullptr>
    void handle_check_enable()
    {
        handle_check();
    }
    template<
        typename Dummy                                           = int,
        typename std::enable_if<!enable_exception, Dummy>::type* = nullptr>
    void handle_check_enable()
    {
    }

    ~generic_handle_t()
    {
        handle_check_enable();
    }

    C_DELETE_COPY_CONSTRUCTOR(generic_handle_t);

    bool operator!() const
    {
        return hnd == hnd_type();
    }

    explicit operator hnd_type() const
    {
        return hnd;
    }

    void release()
    {
        hnd = hnd_type();
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
