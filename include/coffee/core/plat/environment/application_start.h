#ifndef CAPPLICATION_START
#define CAPPLICATION_START

#include "../../types/tdef/fntypes.h"

#ifdef COFFEE_ANDROID
#include <SDL2/SDL.h>
struct android_app;
#endif

int deref_main(Coffee::CoffeeMainWithArgs mainfun, int argc, char** argv);

//This is a cheeky little macro that allows us to wrap the main function.
#ifdef COFFEE_ANDROID
#define COFFEE_APPLICATION_MAIN(mainfun) \
    extern "C" int main(int argv, char** argc){ return deref_main(mainfun,argv,argc); }
#else
#define COFFEE_APPLICATION_MAIN(mainfun) \
    int main(int argv, char** argc){ return deref_main(mainfun,argv,argc); }
#endif

#endif
