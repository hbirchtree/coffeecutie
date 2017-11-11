#pragma once

#include "../../types/tdef/fntypes.h"
#include "../../coffee_version.h"

#if (defined(COFFEE_ANDROID) && defined(COFFEE_USE_SDL2))
#include <jni.h>
#endif

#if defined(COFFEE_SDL_MAIN)
#define main SDL_main
#elif defined(COFFEE_CUSTOM_MAIN)
#define main Coffee_main

#if defined(COFFEE_APPLE_MOBILE)
extern Coffee::CoffeeMainWithArgs apple_entry_point;
#elif defined(COFFEE_ANDROID)
extern Coffee::CoffeeMainWithArgs android_entry_point;
#endif

#endif

extern int deref_main(Coffee::CoffeeMainWithArgs mainfun, int argc, char** argv);

//This is a cheeky little macro that allows us to wrap the main function.
#if defined(COFFEE_SDL_MAIN)

// Wraps it with a custom name defined above
#define COFFEE_APPLICATION_MAIN(mainfun) \
    extern "C" int main(int argv, char** argc){ \
        return deref_main(mainfun,argv,argc); \
    }

#elif defined(COFFEE_CUSTOM_MAIN)

#if defined(COFFEE_APPLE_MOBILE)
// This is loaded from AppDelegate.m in CoffeeWindow_GLKit
#define COFFEE_APPLICATION_MAIN(mainfun) \
    Coffee::CoffeeMainWithArgs apple_entry_point = mainfun;
#elif defined(COFFEE_ANDROID)
#define COFFEE_APPLICATION_MAIN(mainfun) \
    Coffee::CoffeeMainWithArgs android_entry_point = mainfun;
#endif

#else

// Plain old main() function
#define COFFEE_APPLICATION_MAIN(mainfun) \
    int main(int argv, char** argc){ return deref_main(mainfun,argv,argc); }
#endif
