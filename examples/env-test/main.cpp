#include <coffee/CCore>
#include <coffee/core/CPlatform>

#include "library.h"

using namespace Coffee;

using namespace CLibraryLoader;

int32 coffee_main(int32, byte_t**)
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

    _cbasic_version<int32> libver;
    libver.major = 1;
    libver.minor = 0;
    libver.revision = 0;
    CObjectLoader<TestClass>* libtest = coffee_get_lib<TestClass>("test",&libver);

    if(libtest)
    {
        TestClass* impl = libtest->loader();
        impl->printHello();
        coffee_close_lib(libtest);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
