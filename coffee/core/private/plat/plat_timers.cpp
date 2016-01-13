#include <plat/plat_timers.h>

#include <plat/plat_core.h>

Coffee::CElapsedTimerD *Coffee::CFunctional::AllocTimerD()
{
    return new _plat_timer_scalar_micro;
}

Coffee::CElapsedTimerMicro *Coffee::CFunctional::AllocTimerMicro()
{
    return new _plat_timer_ll_micro;
}

Coffee::CElapsedTimer *Coffee::CFunctional::AllocTimer()
{
    return new _plat_timer_ll_milli;
}

void Coffee::CFunctional::FreeTimer(Coffee::CElapsedTimer *timer)
{
    delete timer;
}

void Coffee::CFunctional::FreeTimer(Coffee::CElapsedTimerD *timer)
{
    delete timer;
}
