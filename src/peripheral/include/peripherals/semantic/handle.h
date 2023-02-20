#pragma once

#include <peripherals/constants.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

#include <utility>

namespace semantic {

using namespace stl_types;

namespace handle_modes {

struct no_op
{
};

struct auto_close
{
};

struct exception_on_destruct
{
};

} // namespace handle_modes

template<
    typename hnd_type,
    typename HandleMode          = handle_modes::exception_on_destruct,
    hnd_type InvalidValue        = hnd_type(),
    void (*close_func)(hnd_type) = nullptr>
// clang-format off
requires(
    (type_safety::is_pod_v<hnd_type> || std::is_pointer_v<hnd_type>) &&
    (
        (std::is_same_v<HandleMode, handle_modes::auto_close> &&
         close_func != nullptr) ||
        std::is_same_v<HandleMode, handle_modes::exception_on_destruct> ||
        std::is_same_v<HandleMode, handle_modes::no_op>
    )
)
    // clang-format on
    struct generic_handle_t : non_copy
{
    static constexpr bool exceptions_mode
        = std::is_same<handle_modes::exception_on_destruct, HandleMode>::value;

    using handle_type = hnd_type;

    using borrow = generic_handle_t<hnd_type, handle_modes::no_op>;

    generic_handle_t() : hnd(InvalidValue)
    {
    }

    explicit generic_handle_t(hnd_type handle) : hnd(handle)
    {
    }

    generic_handle_t(generic_handle_t&& other) noexcept
    {
        hnd = std::move(other.hnd);
        other.release();
    }

    void handle_check()
    {
        using namespace handle_modes;

        if constexpr(std::is_same<auto_close, HandleMode>::value)
        {
            if(hnd != InvalidValue)
                close_func(hnd);
        }

        if constexpr(std::is_same<exception_on_destruct, HandleMode>::value)
        {
            if constexpr(compile_info::debug_mode)
                if(hnd != InvalidValue)
                    Throw(resource_leak("resource leakage detected"));
        }
    }

    ~generic_handle_t()
    {
        handle_check();
    }

    bool operator!() const
    {
        return hnd == InvalidValue;
    }

    operator hnd_type const&() const
    {
        return hnd;
    }

    operator hnd_type&()
    {
        return hnd;
    }

    explicit operator bool() const
    {
        return hnd != InvalidValue;
    }

    void release()
    {
        hnd = InvalidValue;
    }

    generic_handle_t& operator=(hnd_type otherHandle) noexcept
    {
        handle_check();
        hnd = otherHandle;
        return *this;
    }

    generic_handle_t& operator=(generic_handle_t&& other) noexcept
    {
        handle_check();
        hnd = other.hnd;
        other.release();
        return *this;
    }

    hnd_type hnd;
};

} // namespace semantic
