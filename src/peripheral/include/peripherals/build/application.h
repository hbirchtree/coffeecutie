#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace info {

using namespace ::libc_types;
using namespace ::stl_types;

struct AppData
{
    std::string organization_name;
    std::string application_name;
    u32     version_code;
};

} // namespace info
} // namespace platform
