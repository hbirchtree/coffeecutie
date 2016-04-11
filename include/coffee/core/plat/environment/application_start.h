#ifndef CAPPLICATION_START
#define CAPPLICATION_START

#include "../../types/tdef/fntypes.h"
#include "../../coffee_version.h"

#ifdef COFFEE_ANDROID
#include <SDL2/SDL.h>
struct android_app;
#endif

int deref_main(Coffee::CoffeeMainWithArgs mainfun, int argc, char** argv);

FORCEDINLINE void _setVerInfo()
{
    Coffee::CoffeeApplicationData.application_name = COFFEE_APPLICATION_NAME;
    Coffee::CoffeeApplicationData.organization_name = COFFEE_ORGANIZATION_NAME;
    Coffee::CoffeeApplicationData.version_code = COFFEE_VERSION_CODE;
}

//This is a cheeky little macro that allows us to wrap the main function.
#ifdef COFFEE_ANDROID
#define COFFEE_APPLICATION_MAIN(mainfun) \
    extern "C" int main(int argv, char** argc){ _setVerInfo(); return deref_main(mainfun,argv,argc); }
#else
#define COFFEE_APPLICATION_MAIN(mainfun) \
    int main(int argv, char** argc){ _setVerInfo(); return deref_main(mainfun,argv,argc); }
#endif

#endif
