#pragma once

#include <peripherals/error/posix.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/types.h>
#include <string.h>

namespace platform::common::posix {

FORCEDINLINE stl_types::Optional<stl_types::String> error_message(int error)
{
    stl_types::Array<char, 255> error_msg = {{}};
#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_APPLE) || defined(COFFEE_ANDROID)
    if(auto ret = ::strerror_r(error, error_msg.data(), error_msg.size());
       ret == 0)
        return stl_types::String(
            error_msg.data(), ::strlen(error_msg.data()));
#else
    if(auto msg = ::strerror_r(error, error_msg.data(), error_msg.size()))
        return stl_types::String(msg);
#endif
    return std::nullopt;
}

FORCEDINLINE stl_types::Optional<stl_types::String> error_message(
    stl_types::Optional<int> error)
{
    return error_message(error.value_or(0));
}

} // namespace platform::common::posix
