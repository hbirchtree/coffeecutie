#ifndef COFFEE_MACROS
#define COFFEE_MACROS

#include "coffee_types.h"
#include <stdio.h>
#include <stdexcept>

#define C_UNUSED(v) do{(void)(v);}while(0);

/*!
 * \brief Assertion method for core values and unit tests
 * \param exp
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
    fprintf(stderr,"COFFEE:CORE:STUB: %s\n",name);
}

/*!
 * \brief For reporting fatal lack of functionality in platform layer, used for features such as file mapping if it is unsupported on a platform.
 * \param name
 */
inline static void C_BADERROR(Coffee::cstring name)
{
    fprintf(stderr,"COFFEE:CORE:FTAL: %s\n",name);
}

#endif //COFFEE_MACROS
