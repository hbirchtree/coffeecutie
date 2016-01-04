#include <coffee/CCore>
#include "library.h"

using namespace Coffee;

using namespace CLibraryLoader;

int32 coffee_main(int32, byte_t**)
{
    {
        /* Check out system directory strings and user data directories */
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
    }

    {
        /* Try loading a shared library, remember to clean up! */
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
    }

    /* Just printing the compiler string */
    cDebug("Compiler: {0}",CoffeeCompilerString);

    {
        /* Testing function slot/signal classes */
        CRect t(0,0,10,10);
        CFunctionSlot<CRect,int32> b(&t,&CRect::area);

        CFunctionSignal<CRect,int32> sigtest;
        cDebug("Signal return: {0}",sigtest.call(b));
    }

    {
        /* Try dissecting a couple of floating-point numbers */
        scalar t1 = 1.0;
        scalar t2 = 0.0;
        scalar t3 = 1.2048295;
        scalar t4 = 1832.2048295;

        scalar_dissect* t1_d = (scalar_dissect*)&t1;
        scalar_dissect* t2_d = (scalar_dissect*)&t2;
        scalar_dissect* t3_d = (scalar_dissect*)&t3;
        scalar_dissect* t4_d = (scalar_dissect*)&t4;

        cDebug("Mantissa: {0}, {1}, {2}, {3}",
               t1_d->mantissa,t2_d->mantissa,
               t3_d->mantissa,t4_d->mantissa);
    }

    {
        /* Testing interactive command line */
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
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
