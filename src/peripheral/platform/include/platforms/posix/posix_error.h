#pragma once

#include <array>
#include <peripherals/error/posix.h>
#include <peripherals/stl/types.h>
#include <string.h>

namespace platform::common::posix {

FORCEDINLINE stl_types::Optional<stl_types::String> error_message(int error)
{
    stl_types::Array<char, 255> error_msg = {{}};
    if(auto msg = ::strerror_r(error, error_msg.data(), error_msg.size()))
        return stl_types::String(msg, libc::str::len(msg));
    return std::nullopt;
}

FORCEDINLINE stl_types::Optional<stl_types::String> error_message(
    stl_types::Optional<int> error)
{
    return error_message(error.value_or(0));
}

} // namespace platform::posix
