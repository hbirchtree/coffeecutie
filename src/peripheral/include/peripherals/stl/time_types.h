#pragma once

#include <chrono>
#include <peripherals/libc/types.h>

namespace stl_types {
namespace Chrono {
using namespace std::chrono;

using seconds_float  = duration<libc_types::scalar>;
using seconds_double = duration<libc_types::bigscalar>;

} // namespace Chrono
} // namespace stl_types
