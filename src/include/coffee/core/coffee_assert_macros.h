#pragma once

#include <csignal>
#include <stdlib.h>
#include "plat/memory/memory_ops.h"

#include "types/tdef/integertypes.h"
#include "coffee_mem_macros.h"

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Coffee{

FORCEDINLINE
void IMPLODETHEUNIVERSE()
{
    std::raise(SIGSEGV);
}

C_NORETURN FORCEDINLINE
void RUNOUTTHEWINDOW()
{
#if defined(COFFEE_USE_EXCEPTIONS)
    throw "I RAN OUT THE WINDOW";
#else
    std::raise(SIGINT);
    abort();
#endif
}

C_NORETURN FORCEDINLINE
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

C_DEPRECATED FORCEDINLINE
void CASSERT_MEM(c_cptr, c_cptr, szptr)
{
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

FORCEDINLINE void C_BREAK()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
    if(getenv("DEBUG_BREAK"))
        std::raise(SIGINT);
#elif defined(COFFEE_EMSCRIPTEN)
    emscripten_debugger();
#endif
}

}
