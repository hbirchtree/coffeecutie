#ifndef WIN_CORE
#define WIN_CORE

#include "../platform_detect.h"

#ifdef COFFEE_WINDOWS

#include <Windows.h>

#include "coffee/core/CTypes"
#include "coffee/core/base/cdebug.h"

namespace Coffee{
namespace CFunctional {
    extern uint64 _win_api_get_time();

}

}

#endif //_WIN32
#endif //WIN_CORE
