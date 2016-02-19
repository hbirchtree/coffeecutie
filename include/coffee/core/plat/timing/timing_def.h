#ifndef COFFEE_CORE_PLAT_TIMING_DEF_H
#define COFFEE_CORE_PLAT_TIMING_DEF_H

#include "../../types/composite_types.h"

namespace Coffee{

struct TimeDef
{
    /*!
     * \brief Current microsecond
     * \return
     */
    static uint64 Microsecond(){return 0;}

    static Timestamp CurrentTimestamp(){return 0;}
    static uint64 CurrentMicroTimestamp(){return 0;}

    static DateTime GetDateTime(Timestamp){return DateTime();}

    static CString StringDate(cstring,DateTime){return "";}
    static CString ClockString(){return "";}
};

}

#endif

#include "windows/timers.h"
#include "unix/timers.h"
