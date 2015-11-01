#ifndef CAPPLICATION_START
#define CAPPLICATION_START
#include "coffee/core/coffee.h"

#define COFFEE_APPLICATION_MAIN(mainfun) \
    int main(int argv, char** argc)try \
    { \
        Coffee::CoffeeInit(); \
        return Coffee::CoffeeMain(mainfun,argv,argc); \
    }catch(std::exception e){ \
        return 1; \
    } \

#endif
