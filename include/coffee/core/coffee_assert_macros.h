#pragma once

#include <csignal>
#include <stdlib.h>
#include "plat/memory/memory_ops.h"

#include "types/tdef/integertypes.h"
#include "coffee_mem_macros.h"

namespace Coffee{

FORCEDINLINE
void IMPLODETHEUNIVERSE()
{
    std::raise(SIGSEGV);
}

FORCEDINLINE
void RUNOUTTHEWINDOW()
{
#if defined(COFFEE_USE_EXCEPTIONS)
    throw "I RAN OUT THE WINDOW";
#else
    std::raise(SIGINT);
#endif
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
    std::raise(SIGABRT);
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
template<typename T>
FORCEDINLINE void C_USED(T const& v)
{
    (void)v;
}

}
