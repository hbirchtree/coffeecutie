#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>

using namespace Coffee;

int32 CoffeeImGui_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("CoffeeImGui/");
    
    cDebug("Hello World!");

    return 0;
}

COFFEE_APPLICATION_MAIN(CoffeeImGui_main)
