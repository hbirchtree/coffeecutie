#include <coffee/CCore>
#include "library.h"

using namespace Coffee;

using namespace CLibraryLoader;

int32 coffee_main(int32, byte_t**)
{
    cstring_w cfg_dir  = env_get_user_data(
                "hbirchtree",
                "Best Coffee of All Time");

    cstring_w app_dir  = env_get_application_dir();
    cstring_w exe_name = executable_name();

    cDebug("Settings directory: {0}",cfg_dir);
    cDebug("Program directory:  {0}",app_dir);
    cDebug("Launching from      {0}",exe_name);

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

    cDebug("Compiler: {0}",CoffeeCompilerString);

    CRect t(0,0,10,10);
    CFunctionSlot<CRect,int32> b(&t,&CRect::area);

    CFunctionSignal<CRect,int32> sigtest;
    cDebug("Signal return: {0}",sigtest.call(b));

    cDebug("Interactive: {0}",interactive_cmd());

    std::string ts;
    ts.resize(100);

    clear_screen();
    while(!c_strcmp(&ts[0],"quit\n"))
    {
        ts.clear();
        fprintf(stderr,"Type something: ");
        read_string(&ts[0],99,stdin);
        cDebug("You wrote: {0}",ts.c_str());
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
