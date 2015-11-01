#include "coffee.h"

#include "base/cdebug.h"

static bool coffee_initialized = false;

namespace Coffee{

void CoffeeInit()
{
    //Allow core dump by default
    coffee_initialized = true;
    coffee_enable_core_dump();
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argv, byte **argc)
{
    if(!coffee_initialized)
        cWarning("CoffeeInit was not called");
    cDebug("CoffeeMain is in use");
    return mainfun(argv,argc);
}

}
