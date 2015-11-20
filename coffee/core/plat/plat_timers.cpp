#include "plat_timers.h"

#include "plat_core.h"

Coffee::CElapsedTimerD *Coffee::CFunctional::coffee_fun_alloc_timerd()
{
    return new _plat_timer_scalar_micro;
}

Coffee::CElapsedTimerMicro *Coffee::CFunctional::coffee_fun_alloc_timer_micro()
{
    return new _plat_timer_ll_micro;
}

Coffee::CElapsedTimer *Coffee::CFunctional::coffee_fun_alloc_timer()
{
    return new _plat_timer_ll_milli;
}
