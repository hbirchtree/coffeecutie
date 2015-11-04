#ifndef CAPPLICATION_START
#define CAPPLICATION_START
#include "coffee/core/coffee.h"

#define COFFEE_APPLICATION_MAIN(mainfun) \
    int main(int argv, char** argc) \
    { \
        Coffee::CoffeeInit(); \
        return Coffee::CoffeeMain(mainfun,argv,argc); \
    }\

#endif
