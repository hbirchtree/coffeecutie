//#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>

#if defined(COFFEE_WINDOWS)
#include <coffee/core/plat/plat_windows.h>
#endif

using namespace Coffee;

int deref_main(CoffeeMainWithArgs mainfun, int argc, char** argv)
{
#if defined(COFFEE_WINDOWS)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    return Coffee::CoffeeMain(mainfun,argc,argv);
}
