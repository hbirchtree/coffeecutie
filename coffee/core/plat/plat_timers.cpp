#include "plat_timers.h"

#include "plat_core.h"

Coffee::CElapsedTimerD *Coffee::CFunctional::coffee_fun_alloc_timerd()
{
#ifdef COFFEE_UNIXPLAT
    return new _posix_CElapsedTimerD();
#else
    C_STUBBED("Elapsed timers");
    return nullptr;
#endif
}

Coffee::CElapsedTimerMicro *Coffee::CFunctional::coffee_fun_alloc_timer_micro()
{
#ifdef COFFEE_UNIXPLAT
    return new _posix_CElapsedTimerMicro();
#else
    C_STUBBED("Elapsed timers (micro)");
    return nullptr;
#endif
}

Coffee::CElapsedTimer *Coffee::CFunctional::coffee_fun_alloc_timer()
{
#ifdef COFFEE_UNIXPLAT
    return new _posix_CElapsedTimer();
#else
    C_STUBBED("Elapsed timers (bigscalar)");
    return nullptr;
#endif
}
