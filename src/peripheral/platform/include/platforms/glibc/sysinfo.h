#pragma once

#include <gnu/libc-version.h>
#include <string_view>

namespace platform::info::os::glibc {

inline std::string_view libc_version()
{
    return gnu_get_libc_version();
}

}
