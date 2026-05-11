#pragma once

#include <peripherals/error/posix.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/types.h>
#include <string.h>

namespace platform::common::posix {

FORCEDINLINE std::optional<std::string> error_message(int error)
{
    std::array<char, 255> error_msg = {{}};
#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_APPLE) || \
    defined(COFFEE_ANDROID)
    if(auto ret = ::strerror_r(error, error_msg.data(), error_msg.size());
       ret == 0)
        return std::string(error_msg.data(), ::strlen(error_msg.data()));
#elif (defined(COFFEE_MINGW64) || defined(COFFEE_MINGW32))
#if __STDC_WANT_SECURE_LIB__
    if(auto ret = ::strerror_s(error_msg.data(), error_msg.size(), error);
       ret == 0)
        return std::string(error_msg.data(), ::strlen(error_msg.data()));
#else
    if(auto ret = ::strerror(error); ret)
        return std::string(ret, ::strlen(ret));
#endif
#else
    if(auto msg = ::strerror_r(error, error_msg.data(), error_msg.size()))
        return std::string(msg);
#endif
    return std::nullopt;
}

FORCEDINLINE std::optional<std::string> error_message(std::optional<int> error)
{
    return error_message(error.value_or(0));
}

} // namespace platform::common::posix
