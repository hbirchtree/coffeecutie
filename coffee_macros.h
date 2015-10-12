#ifndef COFFEE_MACROS
#define COFFEE_MACROS

#include "coffee_types.h"
#include <stdio.h>
#include <stdexcept>

//For unused variables
#define C_UNUSED(v) do{(void)(v);}while(0);

//Assertion for unit tests
inline static void CASSERT(bool exp)
{
    if(!exp)
        throw std::runtime_error("Assert failed");
}

//#define CASSERT(exp) if(!exp)throw std::runtime_error("Assert failed");

//When porting, stubs are nice to have
inline static void C_STUBBED(Coffee::cstring name)
{
    fprintf(stderr,"COFFEE:CORE:STUB: %s\n",name);
}

//#define C_STUBBED(name) fprintf(stderr,"COFFEE:CORE:STUB: %s\n",name)
inline static void C_BADERROR(Coffee::cstring name)
{
    fprintf(stderr,"COFFEE:CORE:FTAL: %s\n",name);
}

//#define C_BADERROR(name) fprintf(stderr,"COFFEE:CORE:ERR: %s\n",name)


#endif //COFFEE_MACROS
