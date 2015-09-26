#ifndef COFFEE_MACROS
#define COFFEE_MACROS

#include <stdio.h>

//For unused variables
#define C_UNUSED(v) do{(void)(v);}while(0);

//Assertion for unit tests
#define CASSERT(exp) if(!exp)throw std::runtime_error("Assert failed");

//When porting, stubs are nice to have
#define C_STUBBED(name) fprintf(stderr,"COFFEE:CORE:STUB: %s\n",name)

#define C_BADERROR(name) fprintf(stderr,"COFFEE:CORE:ERR: %s\n",name)


#endif //COFFEE_MACROS
