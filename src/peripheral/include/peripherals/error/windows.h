#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace win32 {

std::string error_to_string(libc_types::u32 err);
std::string last_error();

} // namespace win32
} // namespace platform
