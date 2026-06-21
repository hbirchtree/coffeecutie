#pragma once

#include <peripherals/error/result.h>
#include <url/url.h>

#include <optional>
#include <string>

// The Gamecube/Wii has no environment variables. These are stubs so that the
// generic platform::env API compiles and degrades gracefully.

namespace platform::env::gekko {

using stl_types::failure;
using stl_types::result;
using stl_types::success;

using gekko_error = int;

FORCEDINLINE std::optional<std::string> var(std::string const&)
{
    return std::nullopt;
}

FORCEDINLINE std::optional<gekko_error> set_var(
    std::string const&, std::optional<std::string> const&)
{
    return std::nullopt;
}

FORCEDINLINE result<url::Url, gekko_error> home_dir()
{
    return success(url::constructors::MkSysUrl("/"));
}

} // namespace platform::env::gekko
