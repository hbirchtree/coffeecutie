#pragma once

#include <coffee/components/components.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>

#include "services.h"

namespace comp_app {

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
