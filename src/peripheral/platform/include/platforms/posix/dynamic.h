#pragma once

#include <peripherals/constants.h>
#include <peripherals/error/result.h>
#include <peripherals/semantic/handle.h>
#include <peripherals/stl/types.h>
#include <platforms/base/dynamic.h>

#include "posix_error.h"

#include <dlfcn.h>

namespace platform::dl::posix {

using common::posix::collect_error;
using common::posix::error_message;
using common::posix::posix_error;
using stl_types::failure;
using stl_types::result;
using stl_types::success;

using stl_types::String;

namespace detail {

FORCEDINLINE void close_dl_handle(void* handle)
{
    if(auto stat = dlclose(handle); stat != 0)
    {
        if(auto error = dlerror())
            Throw(undefined_behavior(error));
        else
        {
            if(auto msg = error_message(common::posix::get_error());
               msg.has_value())
                Throw(undefined_behavior(msg.value()));
        }
    }
}

FORCEDINLINE String error()
{
    if(auto error = dlerror(); !error)
        return {};
    else
        return error;
}

FORCEDINLINE int from_open_flags(load_flags flags)
{
    switch(flags)
    {
    case load_flags::global:
        return RTLD_GLOBAL;
    default:
        return 0;
    }
}

} // namespace detail

using dl_error = String;

using dl_handle_t = semantic::generic_handle_t<
    void*,
    semantic::handle_modes::auto_close,
    nullptr,
    detail::close_dl_handle>;

struct dynamic_lib
{
    dl_handle_t handle;
};

static constexpr auto library_prefix = "lib";
static constexpr auto library_suffix =
    compile_info::platform::is_macos ? ".dylib" : ".so";

FORCEDINLINE result<dynamic_lib, dl_error> load(
    String const& name, load_params_t const& params = {})
{
    auto load_name = library_prefix + name + library_suffix;

    if(auto handle = dlopen(
           name.c_str(), RTLD_NOW | detail::from_open_flags(params.flags));
       !handle)
    {
        return failure(detail::error());
    } else
    {
        return success(dynamic_lib{
            .handle = dl_handle_t(handle),
        });
    }
}

template<typename ReturnType, typename... Args>
FORCEDINLINE result<stl_types::Function<ReturnType(Args...)>, dl_error>
             symbol_from(dynamic_lib& lib, String const& symbol)
{
    using function_type = stl_types::Function<ReturnType(Args...)>;
    using pointer_type  = ReturnType (*)(Args...);

    if(auto symbol = dlsym(lib.handle, symbol.c_str()); !symbol)
        return failure(detail::error());
    else
    {
        auto f_ptr = reinterpret_cast<pointer_type>(symbol);
        return success(function_type(f_ptr));
    }
}

} // namespace platform::dl::posix