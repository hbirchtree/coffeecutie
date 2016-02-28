#ifndef COFFEE_MACROS
#define COFFEE_MACROS

#include "types/tdef/integertypes.h"
#include "plat/plat_memory.h"
#include "coffee_mem_macros.h"

#undef major
#undef minor
#undef far
#undef near

#include "plat/platform_detect.h"
#include <stdio.h>
#include <stdexcept>

#ifdef __CBUILDTIME__
#define __CBDATETIME__ __CBUILDTIME__
#else
#define __CBDATETIME__ "00.00.00.0000"
#endif

#define C_CONSTRUCT_BUILD_STRING(ver,datetime) ver "." datetime

namespace Coffee{

template<typename T>
/*!
 * \brief Use this to mark variables as unused to avoid compiler warnings. We can get away with this.
 */
FORCEDINLINE void C_UNUSED(T const&)
{
}

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
    CASSERT(CMemCmp(m1,m2,size));
}

/*!
 * \brief This is used for marking functionality as stubbed, used for smaller, less important features.
 * \param name
 */
FORCEDINLINE void C_STUBBED(cstring name)
{
    fprintf(DefaultDebugOutputPipe,"STUBD:%s@%i: %s\n",
            __FILE__,
            __LINE__,
            name);
}

FORCEDINLINE void C_FIXME(cstring identifier)
{
    fprintf(DefaultDebugOutputPipe,"FIXME:%s@%i: %s\n",
            __FILE__,
            __LINE__,
            identifier);
}

FORCEDINLINE void C_PERFWARN(cstring file, int line, cstring msg)
{
    fprintf(DefaultDebugOutputPipe,"PERFW:%s@%i: %s\n",file,line,msg);
}

/*!
 * \brief For reporting fatal lack of functionality in platform layer, used for features such as file mapping if it is unsupported on a platform.
 * \param name
 */
FORCEDINLINE void C_BADERROR(cstring name)
{
    fprintf(DefaultDebugOutputPipe,"FATAL:%s@%i: %s\n",
            __FILE__,
            __LINE__,
            name);
}

FORCEDINLINE void C_DEPRECATED()
{
    fprintf(DefaultDebugOutputPipe,"DEPRECATED");
}
}

#endif //COFFEE_MACROS
