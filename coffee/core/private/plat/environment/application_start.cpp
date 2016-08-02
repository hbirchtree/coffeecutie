//#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>
#include <coffee/core/CProfiling>

#if defined(COFFEE_WINDOWS)
#include <coffee/core/plat/plat_windows.h>
#elif defined(COFFEE_ANDROID)

#endif

using namespace Coffee;

int deref_main(CoffeeMainWithArgs mainfun, int argc, char** argv)
{
#if defined(COFFEE_WINDOWS)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#elif defined(COFFEE_ANDROID)
    Profiler::ResetPointers();
#endif
    return Coffee::CoffeeMain(mainfun,argc,argv);
}
