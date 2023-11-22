#pragma once

#include "../posix/detail.h"
#include <peripherals/error/result.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <url/url.h>

namespace platform::file::win32::path {

FORCEDINLINE result<Url, posix::posix_error> executable()
{
    return url::constructors::MkUrl(".");
}

FORCEDINLINE result<Url, posix::posix_error> app_dir()
{
    return url::constructors::MkUrl(".");
}

} // namespace platform::file::win32::path
