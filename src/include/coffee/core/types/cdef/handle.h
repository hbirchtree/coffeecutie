#pragma once

#include "../tdef/integertypes.h"
#include "../tdef/stltypes.h"

namespace Coffee {

template<
    typename hnd_type,
    typename std::enable_if<
        std::is_trivially_copyable<hnd_type>::value>::type* = nullptr>
struct generic_handle_t : non_copy
{
    generic_handle_t() : hnd(hnd_type())
    {
    }

    explicit generic_handle_t(hnd_type handle) : hnd(handle)
    {
    }

    template<
        typename Dummy = int,
        typename std::enable_if<std::is_integral<hnd_type>::value, Dummy>::
            type* = nullptr>
    void handle_check()
    {
#ifndef NDEBUG
        if(hnd != hnd_type(0))
            Throw(resource_leak("resource leakage detected"));
#endif
    }

    template<
        typename Dummy = int,
        typename std::enable_if<!std::is_integral<hnd_type>::value, Dummy>::
            type* = nullptr>
    void handle_check()
    {
    }

    ~generic_handle_t()
    {
        handle_check();
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
        hnd = 0;
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

    u32 hnd;
};

} // namespace Coffee
