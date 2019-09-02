#pragma once

#include <coffee/components/components.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>

#include "services.h"

namespace comp_app {
namespace detail {
using namespace Coffee::Components;
}

enum class AppError
{
    None = 0,
};

struct application_error_category : stl_types::error_category
{
    virtual ~application_error_category();

    virtual const char* name() const noexcept;
    virtual std::string message(int ec) const;
};

using app_error =
    stl_types::domain_error_code<AppError, application_error_category>;

template<typename T>
using AppValue = detail::Globals::ValueSubsystem<T>;

using AppLoaderTag = detail::TagType<libc_types::u32, libc_types::u32>;

struct AppLoader : detail::RestrictedSubsystem<
                               AppLoaderTag,
                               detail::TypeList<void>,
                               detail::TypeList<void>,
                               detail::TypeList<Windowing>>
{
    using services = detail::TypeList<void>;

    bool init(Proxy& p)
    {
        p.service<Windowing>();

        return true;
    }
};

} // namespace comp_app
