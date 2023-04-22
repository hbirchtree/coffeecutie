#pragma once

#include <peripherals/stl/types.h>

#include <sys/utsname.h>

namespace platform::info::os::posix {

inline std::string kernel()
{
    utsname data{};
    if(uname(&data) != 0)
        return {};
    return data.sysname;
}

inline std::string kernel_version()
{
    utsname data{};
    if(uname(&data) != 0)
        return {};
    return data.release;
}

inline std::string architecture()
{
    utsname data{};
    if(uname(&data) != 0)
        return {};
    return data.machine;
}

}
