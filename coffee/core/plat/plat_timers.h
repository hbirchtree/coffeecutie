#ifndef COFFEE_PLAT_TIMERS_H
#define COFFEE_PLAT_TIMERS_H

#include "coffee/core/ctypes/composite_types.h"

namespace Coffee{
namespace CFunctional{

/*!
 * \brief Will return a concealed, platform-dependent, general-purpose timer.
 * \return A pointer to said timer
 */
extern CElapsedTimer* coffee_fun_alloc_timer();

/*!
 * \brief Will return a concealed, platform-dependent, accurate timer, with varying accuracy based on hardware.
 * \return A pointer to said timer
 */
extern CElapsedTimerMicro* coffee_fun_alloc_timer_micro();

/*!
 * \brief Will return a concealed, platform-dependent, general-purpose timer with double format.
 * \return A pointer to said timer
 */
extern CElapsedTimerD* coffee_fun_alloc_timerd();

}
}

#endif
