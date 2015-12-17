#ifndef COFFEE_MACROS
#define COFFEE_MACROS

#include "coffee/core/plat/platform_detect.h"
#include "plat/cmemory.h"
#include "types/types.h"
#include <stdio.h>
#include <stdexcept>

#define C_DELETE_COPY_CONSTRUCTOR(ctype) ctype(ctype const&) = delete

namespace Coffee{
#if defined(COFFEE_WINDOWS)

constexpr Coffee::cstring print_color_debug = "";
constexpr Coffee::cstring print_color_warning = "";
constexpr Coffee::cstring print_color_stub = "";
constexpr Coffee::cstring print_color_fatal = "";

constexpr Coffee::cstring print_color_reset = "";

#else
constexpr Coffee::cstring print_color_debug = "\033[1m\033[44m";
constexpr Coffee::cstring print_color_warning = "\033[1m\033[43m";
constexpr Coffee::cstring print_color_stub = "\033[1m\033[42m";
constexpr Coffee::cstring print_color_fatal = "\033[1m\033[41m";

constexpr Coffee::cstring print_color_reset = "\033[0m";
#endif


/*!
 * \brief Use this to mark variables as unused to avoid compiler warnings. We can get away with this.
 */
inline static void C_UNUSED(...)
{
}

/*!
 * \brief Assertion method for core values and unit tests
 * \param expr Expression which triggers an exception on failure
 */
inline void CASSERT(bool expr)
{
    if(!expr)
        throw std::runtime_error("Assert failed");
}


inline void CASSERT_MEM(c_cptr m1, c_cptr m2, szptr size)
{
    CASSERT(c_memcmp(m1,m2,size));
}

/*!
 * \brief This is used for marking functionality as stubbed, used for smaller, less important features.
 * \param name
 */
inline void C_STUBBED(cstring name)
{
    fprintf(stderr,"%sCOFFEE:CORE:STUB%s: %s\n",
            print_color_fatal,
            print_color_reset,
            name);
}

inline void C_FIXME(cstring identifier)
{
    fprintf(stderr,"%sCOFFEE:CORE:FIXME%s: %s\n",
            print_color_fatal,
            print_color_reset,
            identifier);
}

/*!
 * \brief For reporting fatal lack of functionality in platform layer, used for features such as file mapping if it is unsupported on a platform.
 * \param name
 */
inline void C_BADERROR(cstring name)
{
    fprintf(stderr,"%sCOFFEE:CORE:FTAL%s: %s\n",
            print_color_fatal,
            print_color_reset,
            name);
}
}

#endif //COFFEE_MACROS
