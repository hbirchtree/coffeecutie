#include <coffee/Core>
#include <coffee/core/plat/application_start.h>

int32 coffee_main(int32, byte**)
{
    cstring_w cfg_dir  = coffee_get_userdata_dir(
                "hbirchtree",
                "Best Coffee of All Time");

    cstring_w app_dir  = coffee_get_application_dir();
    cstring_w exe_name = coffee_executable_name();

    cDebug("Settings directory: %s",cfg_dir);
    cDebug("Program directory:  %s",app_dir);
    cDebug("Launching from      %s",exe_name);

    CResources::coffee_file_mkdir(cfg_dir,true);

    free(cfg_dir);
    free(app_dir);
    free(exe_name);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
