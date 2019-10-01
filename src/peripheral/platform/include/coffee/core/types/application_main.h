#pragma once

#include <peripherals/libc/types.h>

#if defined(COFFEE_GEKKO)
using CoffeeMainWithArgs = int(*)(int, char**);
#else
using CoffeeMainWithArgs = libc_types::i32(*)(libc_types::i32, char**);
#endif
