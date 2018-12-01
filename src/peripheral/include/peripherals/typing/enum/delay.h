#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace semantic {

enum class Delay : libc_types::u8
{
    Wait,
    NoWait
};
}
