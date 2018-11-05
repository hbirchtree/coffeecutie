#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {

struct AppData
{
    CString organization_name;
    CString application_name;

    u32 version_code;
};

} // namespace Coffee
