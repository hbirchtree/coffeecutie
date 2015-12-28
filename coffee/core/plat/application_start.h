#ifndef CAPPLICATION_START
#define CAPPLICATION_START
#include "coffee/core/coffee.h"

//This is a cheeky little macro that allows us to wrap the main function.
#define COFFEE_APPLICATION_MAIN(mainfun) \
    int main(int argv, char** argc){ return Coffee::CoffeeMain(mainfun,argv,argc); }

#endif
