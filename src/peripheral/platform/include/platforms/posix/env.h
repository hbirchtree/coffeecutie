#pragma once

#include <peripherals/base.h>
#include <peripherals/error/result.h>
#include <peripherals/stl/types.h>
#include <platforms/file.h>
#include <url/url.h>

#include <stdlib.h>

namespace platform::env::posix {

using stl_types::String;
using stl_types::Optional;
using stl_types::result;
using stl_types::success;
using stl_types::failure;

using posix_error = int;

FORCEDINLINE Optional<String> var(String const& var)
{
    if(auto out = ::secure_getenv(var.c_str()))
        return out;
    return std::nullopt;
}

FORCEDINLINE Optional<posix_error> set_var(
    String const& var, Optional<String> const& val)
{
    if(val.has_value())
    {
        if(auto res = ::setenv(var.c_str(), val.value().c_str(), 1))
            return std::nullopt;
    } else
        ::unsetenv(var.c_str());
    return std::nullopt;
}

FORCEDINLINE result<url::Url, posix_error> home_dir()
{
    if(auto home = var("HOME"); home.has_value())
        return success(url::constructors::MkSysUrl(home.value()));
    else
        return failure(EINVAL);
}

} // namespace platform::env::posix
