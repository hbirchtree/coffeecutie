#ifndef COFFEE_PLAT_TIMERS_H
#define COFFEE_PLAT_TIMERS_H

#include "coffee/core/types/composite_types.h"

namespace Coffee{
namespace CFunctional{

/*!
 * \brief Will return a millisecond timer
 * \return A pointer to said timer
 */
extern CElapsedTimer* coffee_fun_alloc_timer();

/*!
 * \brief Will return a micro-second timer
 * \return A pointer to said timer
 */
extern CElapsedTimerMicro* coffee_fun_alloc_timer_micro();

/*!
 * \brief Will return a microsecond timer in double format, where 1.0 is equivalent to 1 second
 * \return A pointer to said timer
 */
extern CElapsedTimerD* coffee_fun_alloc_timerd();

extern void coffee_fun_free(CElapsedTimer* timer);

extern void coffee_fun_free(CElapsedTimerD* timer);

}
}

#endif
