#ifndef COFFEE_PLAT_TIMERS_H
#define COFFEE_PLAT_TIMERS_H

#include "coffee/core/types/composite_types.h"

namespace Coffee{
namespace CFunctional{

/*!
 * \brief Will return a millisecond timer
 * \return A pointer to said timer
 */
extern CElapsedTimer* AllocTimer();

/*!
 * \brief Will return a micro-second timer
 * \return A pointer to said timer
 */
extern CElapsedTimerMicro* AllocTimerMicro();

/*!
 * \brief Will return a microsecond timer in double format, where 1.0 is equivalent to 1 second
 * \return A pointer to said timer
 */
extern CElapsedTimerD* AllocTimerD();

extern void FreeTimer(CElapsedTimer* timer);

extern void FreeTimer(CElapsedTimerD* timer);

}
}

#endif
