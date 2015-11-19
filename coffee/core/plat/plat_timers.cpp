#include "plat_timers.h"

#include "plat_core.h"

Coffee::CElapsedTimerD *Coffee::CFunctional::coffee_fun_alloc_timerd()
{
#if defined(COFFEE_UNIXPLAT)
    return new _posix_CElapsedTimerD();
#elif defined(COFFEE_WINDOWS)
	return new _windows_CElapsedTimerD();
#else
    C_STUBBED("Elapsed timers");
    return nullptr;
#endif
}

Coffee::CElapsedTimerMicro *Coffee::CFunctional::coffee_fun_alloc_timer_micro()
{
#if defined(COFFEE_UNIXPLAT)
    return new _posix_CElapsedTimerMicro();
#elif defined(COFFEE_WINDOWS)
	return new _windows_CElapsedTimerMicro();
#else
    C_STUBBED("Elapsed timers (micro)");
    return nullptr;
#endif
}

Coffee::CElapsedTimer *Coffee::CFunctional::coffee_fun_alloc_timer()
{
#if defined(COFFEE_UNIXPLAT)
	return new _posix_CElapsedTimer();
#elif defined(COFFEE_WINDOWS)
	return new _windows_CElapsedTimer();
#else
    C_STUBBED("Elapsed timers (bigscalar)");
    return nullptr;
#endif
}
