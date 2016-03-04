#pragma once

#include "types/tdef/integertypes.h"
#include "coffee_mem_macros.h"

namespace Coffee{

/*!
 * \brief Assertion method for core values and unit tests
 * \param expr Expression which triggers an exception on failure
 */
FORCEDINLINE void CASSERT(bool expr)
{
#ifdef COFFEE_USE_EXCEPTIONS
    if(!expr)
        throw std::runtime_error("Assert failed");
#else
    if(!expr)
        abort();
#endif
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