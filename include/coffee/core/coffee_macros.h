#ifndef COFFEE_MACROS
#define COFFEE_MACROS

#include "types/basetypes.h"
#include "plat/plat_memory.h"
#include "coffee_mem_macros.h"
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
#if defined(COFFEE_WINDOWS)

constexpr cstring print_color_debug = "";
constexpr cstring print_color_warning = "";
constexpr cstring print_color_stub = "";
constexpr cstring print_color_fatal = "";

constexpr cstring print_color_reset = "";

#else
constexpr cstring print_color_debug =   "\033[1m\033[44m";
constexpr cstring print_color_warning = "\033[1m\033[43m";
constexpr cstring print_color_stub =    "\033[1m\033[42m";
constexpr cstring print_color_fatal =   "\033[1m\033[41m";

constexpr cstring print_color_reset = "\033[0m";
#endif


template<typename... Args>
/*!
 * \brief Use this to mark variables as unused to avoid compiler warnings. We can get away with this.
 */
FORCEDINLINE void C_UNUSED(Args...)
{
}

/*!
 * \brief Assertion method for core values and unit tests
 * \param expr Expression which triggers an exception on failure
 */
FORCEDINLINE void CASSERT(bool expr)
{
    if(!expr)
        throw std::runtime_error("Assert failed");
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
    fprintf(DefaultDebugOutputPipe,"%sCOFFEE:CORE:STUB%s: %s\n",
            print_color_fatal,
            print_color_reset,
            name);
}

FORCEDINLINE void C_FIXME(cstring identifier)
{
    fprintf(DefaultDebugOutputPipe,"%sCOFFEE:CORE:FIXME%s: %s\n",
            print_color_fatal,
            print_color_reset,
            identifier);
}

FORCEDINLINE void C_PERFWARN(cstring file, int line, cstring msg)
{
    fprintf(DefaultDebugOutputPipe,"Perf warning:%s@%i: %s\n",file,line,msg);
}

/*!
 * \brief For reporting fatal lack of functionality in platform layer, used for features such as file mapping if it is unsupported on a platform.
 * \param name
 */
FORCEDINLINE void C_BADERROR(cstring name)
{
    fprintf(DefaultDebugOutputPipe,"%sCOFFEE:CORE:FTAL%s: %s\n",
            print_color_fatal,
            print_color_reset,
            name);
}
}

#endif //COFFEE_MACROS
