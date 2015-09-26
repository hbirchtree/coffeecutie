#ifndef ARGUMENT_PARSE
#define ARGUMENT_PARSE

#include "plat/platform_detect.h"

#include "coffee_macros.h"
#include "coffee_types.h"

using namespace Coffee;

#if defined(COFFEE_LINUX)

#include <stdlib.h>

static cstring coffee_executable_name()
{
    return realpath("/proc/self/exe",nullptr);
}

#elif defined(COFFEE_WINDOWS)

static cstring coffee_executable_name();

#endif

#endif
