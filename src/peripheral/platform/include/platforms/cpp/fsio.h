#pragma once

#include "../posix/fsio.h"

#include <filesystem>

namespace platform::file::cpp {

FORCEDINLINE std::optional<posix::posix_error> create_directory(
    Url const& file, create_params_t const& params)
{
    auto resolved = *file;
    std::error_code ec;
    if(!std::filesystem::create_directories(resolved, ec))
        return ENOENT;
    else
        return std::nullopt;
}

FORCEDINLINE std::optional<posix::posix_error> create(
    Url const& file, create_params_t const& params)
{
    switch(params.mode)
    {
    case mode_t::directory:
        return create_directory(file, params);
    default:
        return posix::create(file, params);
    }
}

}
