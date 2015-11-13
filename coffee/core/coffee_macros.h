#ifndef COFFEE_MACROS
#define COFFEE_MACROS

#include "coffee_types.h"
#include <stdio.h>
#include <stdexcept>

constexpr Coffee::cstring print_color_debug = "\033[1m\033[44m";
constexpr Coffee::cstring print_color_warning = "\033[1m\033[43m";
constexpr Coffee::cstring print_color_stub = "\033[1m\033[42m";
constexpr Coffee::cstring print_color_fatal = "\033[1m\033[41m";

constexpr Coffee::cstring print_color_reset = "\033[0m";

//#define C_UNUSED(v) do{(void)(v);}while(0);

/*!
 * \brief Use this to mark variables as unused to avoid compiler warnings
 */
inline static void C_UNUSED(...)
{
}

/*!
 * \brief Assertion method for core values and unit tests
 * \param expr Expression which triggers an exception on failure
 */
inline static void CASSERT(bool expr)
{
    if(!expr)
        throw std::runtime_error("Assert failed");
}

/*!
 * \brief This is used for marking functionality as stubbed, used for smaller, less important features.
 * \param name
 */
inline static void C_STUBBED(Coffee::cstring name)
{
    fprintf(stderr,"%sCOFFEE:CORE:STUB%s: %s\n",print_color_fatal,print_color_reset,name);
}

/*!
 * \brief For reporting fatal lack of functionality in platform layer, used for features such as file mapping if it is unsupported on a platform.
 * \param name
 */
inline static void C_BADERROR(Coffee::cstring name)
{
    fprintf(stderr,"%sCOFFEE:CORE:FTAL%s: %s\n",print_color_fatal,print_color_reset,name);
}

#endif //COFFEE_MACROS
