#ifndef COFFEE_PLAT_CORE
#define COFFEE_PLAT_CORE

#include <stdlib.h>
#include "platform_detect.h"
#include "../coffee_macros.h"

namespace Coffee{

} //Coffee

//Function and class definitions are given by the platform-specific headers

#if defined(COFFEE_UNIXPLAT)
#include "unix/unix_core.h"
#elif defined(COFFEE_WINDOWS)
#include "coffee_windows/win_core.h"
#elif defined(__APPLE__) && defined(__MACH__)

#else

#endif

#endif
