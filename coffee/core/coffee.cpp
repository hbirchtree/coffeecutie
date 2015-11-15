#include "coffee.h"

#include "base/cdebug.h"
#include "coffee/core/plat/plat_core.h"
#include "coffee/core/unit_tests/data_types.h"

static bool coffee_initialized = false;

namespace Coffee{

void CoffeeInit()
{
    coffee_initialized = true;
    //Allow core dump by default
    coffee_enable_core_dump();
    CoffeeTests::run_tests();
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argv, byte **argc)
{
    cDebug("Build date: %s, %s",build_date,build_time);
    if(!coffee_initialized)
        cWarning("CoffeeInit was not called");
    cDebug("CoffeeMain is in use");
    return mainfun(argv,argc);
}

}
