#ifndef COFFEE_MACROS
#define COFFEE_MACROS

#include "plat/platform_detect.h"
#include <stdio.h>
#include <stdexcept>

#ifdef __CBUILDTIME__
#define __CBDATETIME__ __CBUILDTIME__
#else
#define __CBDATETIME__ "00.00.00.0000"
#endif

#define C_CONSTRUCT_BUILD_STRING(ver,datetime) ver "." datetime

#include "coffee_assert_macros.h"
#include "coffee_message_macros.h"

#endif //COFFEE_MACROS
