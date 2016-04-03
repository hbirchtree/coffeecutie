#pragma once

#include <stdlib.h>
#include <stdexcept>
#include "plat/memory/memory_ops.h"

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
    RUNOUTTHEWINDOW();
}

FORCEDINLINE
/*!
 * \brief ABORT!
 */
void ABORTEVERYTHINGGOGOGO()
{
    /* ABORT! */
    abort();
}

FORCEDINLINE
/*!
 * \brief Assertion method for core values and unit tests
 * \param expr Expression which triggers an exception on failure
 */
void CASSERT(bool expr)
{
    if(!expr)
        ABORTEVERYTHINGGOGOGO();
}

FORCEDINLINE
void CASSERT_MEM(c_cptr m1, c_cptr m2, szptr size)
{
    CASSERT(Mem::MemCmp(m1,m2,size));
}

template<typename T> FORCEDINLINE
/*!
 * \brief Use this to mark variables as unused to avoid compiler warnings. We can get away with this.
 */
void C_UNUSED(T const&)
{
}

}
