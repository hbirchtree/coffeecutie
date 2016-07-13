#ifndef CAPPLICATION_START
#define CAPPLICATION_START

#include "../../types/tdef/fntypes.h"
#include "../../coffee_version.h"

#if defined(COFFEE_ANDROID) && !defined(ANDROID_DONT_USE_SDL2)
#include <SDL2/SDL.h>
#endif

extern int deref_main(Coffee::CoffeeMainWithArgs mainfun, int argc, char** argv);

FORCEDINLINE void _setVerInfo()
{
#ifndef COFFEE_APPLICATION_LIBRARY
    Coffee::CoffeeApplicationData.application_name = COFFEE_APPLICATION_NAME;
    Coffee::CoffeeApplicationData.organization_name = COFFEE_ORGANIZATION_NAME;
    Coffee::CoffeeApplicationData.version_code = COFFEE_VERSION_CODE;
#endif
}

//This is a cheeky little macro that allows us to wrap the main function.
#if defined(COFFEE_ANDROID)
#define COFFEE_APPLICATION_MAIN(mainfun) \
    Coffee::CoffeeMainWithArgs android_entry_point = mainfun; \
    extern "C" int main(int argv, char** argc){ _setVerInfo(); return deref_main(mainfun,argv,argc); }
#else
#define COFFEE_APPLICATION_MAIN(mainfun) \
    int main(int argv, char** argc){ _setVerInfo(); return deref_main(mainfun,argv,argc); }
#endif

#endif
