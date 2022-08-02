#pragma once

#include <peripherals/stl/types.h>

#include <sys/utsname.h>

namespace platform::info::os::posix {

inline stl_types::String kernel()
{
    utsname data{};
    if(uname(&data) != 0)
        return {};
    return data.sysname;
}

inline stl_types::String kernel_version()
{
    utsname data{};
    if(uname(&data) != 0)
        return {};
    return data.release;
}

inline stl_types::String architecture()
{
    utsname data{};
    if(uname(&data) != 0)
        return {};
    return data.machine;
}

}
