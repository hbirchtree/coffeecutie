#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>

using namespace Coffee;

int deref_main(CoffeeMainWithArgs mainfun, int argc, char** argv)
{
    return Coffee::CoffeeMain(mainfun,argc,argv);
}