#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace platform::dl {

using libc_types::u32;

enum class load_flags
{
    none   = 0x0,
    global = 0x1,
};

struct load_params_t
{
    load_flags flags = load_flags::none;
    u32        major = 1, minor = 0, revision = 0;
};

} // namespace platform::dl
