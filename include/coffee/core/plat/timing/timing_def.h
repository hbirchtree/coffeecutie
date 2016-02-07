#ifndef COFFEE_CORE_PLAT_TIMING_DEF_H
#define COFFEE_CORE_PLAT_TIMING_DEF_H

#include <coffee/core/CTypes>

namespace Coffee{

struct TimeDef
{
    static Timestamp CurrentTimestamp(){return 0;}
    static DateTime GetDateTime(Timestamp){return DateTime();}

    static CString StringDate(cstring,DateTime){return "";}
    static CString ClockString(){return "";}
};

}

#endif

#include "windows/timers.h"
#include "unix/timers.h"