#include "coffee.h"

#include "base/cdebug.h"
#include "coffee/core/plat/plat_core.h"
#include "coffee/core/unit_tests/data_types.h"
#include "graphics/cgraphics_quirks.h"

static bool coffee_initialized = false;

namespace Coffee{

void CoffeeInit()
{
    coffee_initialized = true;
    //Allow core dump by default
    coffee_enable_core_dump();
#ifndef NDEBUG
    //Run unit tests, ensuring that the system and compilation is sane
    CoffeeTests::run_tests();
#endif
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argv, byte **argc)
{
#ifndef NDEBUG
    cDebug("Build date: %s, %s",_coffee_build_date,_coffee_build_time);
#endif
    if(!coffee_initialized)
        cWarning("CoffeeInit was not called");
    cDebug("CoffeeMain is in use");
    return mainfun(argv,argc);
}

}
