#pragma once

#include "types/tdef/integertypes.h"
#include "coffee_mem_macros.h"

namespace Coffee{

FORCEDINLINE
void RUNOUTTHEWINDOW()
{
    exit(-1);
}

FORCEDINLINE
void SHITONTHEFLOOR()
{
    throw std::runtime_error("*shit*");
}

FORCEDINLINE
/*!
 * \brief ABORT!
 */
void ABORTEVERYTHINGGOGOGO()
{
    /* ABORT! */
#ifdef COFFEE_USE_EXCEPTIONS
    throw std::runtime_error("Shit! Everything's on a cob! We need to leave!");
#else
    /* ABORT! */
    abort();
#endif
}

/*!
 * \brief Assertion method for core values and unit tests
 * \param expr Expression which triggers an exception on failure
 */
FORCEDINLINE void CASSERT(bool expr)
{
    if(!expr)
        ABORTEVERYTHINGGOGOGO();
}

FORCEDINLINE void CASSERT_MEM(c_cptr m1, c_cptr m2, szptr size)
{
    CASSERT(MemCmp(m1,m2,size));
}

template<typename T>
/*!
 * \brief Use this to mark variables as unused to avoid compiler warnings. We can get away with this.
 */
FORCEDINLINE void C_UNUSED(T const&)
{
}

}