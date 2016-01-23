#include <coffee/CCore>
#include <coffee/CImage>
#include "library.h"

using namespace Coffee;

using namespace CLibraryLoader;

int32 coffee_main(int32, cstring_w*)
{
    AltScreen();
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

        CResources::FileMkdir(cfg_dir,true);

        CFree(cfg_dir);
        CFree(app_dir);
        CFree(exe_name);
    }

    cDebug("uint64 size: {0}",sizeof(uint64));
    cDebug("int64 size: {0}",sizeof(int64));
    cDebug("uint32 size: {0}",sizeof(uint32));
    cDebug("int32 size: {0}",sizeof(int32));

    {
        int64 ptr = 123;
        cDebug("Pointer: {0}",(const void* const&)ptr);
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
        cDebug("Perlin values: ");
        for(scalar x=0;x<8;x+=0.1)
        {
            cBasicPrintNoNL("{0} ",NoiseGen::Perlin<scalar,int32,3>(CVec3(x,0,0)));
        }
        cBasicPrint("");
    }

    CSize tsize = TerminalSize();
    cDebug("Terminal size: {0}x{1}",tsize.w,tsize.h);

//    {
//        /* Testing interactive command line */
//        std::string ts;
//        ts.resize(100);

//        while(!CStrCmp(&ts[0],"quit\n"))
//        {
//            ts.clear();
//            cBasicPrintNoNL("Type something: ");
//            ReadString(&ts[0],99,stdin);
//            cDebug("You wrote: {0}",ts.c_str());
//        }
//    }
    ResetScreen();

    {
        cDebug("Testing multithread + printing, should be mutex-locked");
        std::function<void()> message_test = [](){
            int64 msgs = 0;
            ThreadId thread;

            while(msgs<10)
            {
                cDebug("{0}: Spam! Messages! Yes!",thread.hash());
                msgs++;
            }
        };

        auto t1 = CThreading::RunAsync(message_test);
        auto t2 = CThreading::RunAsync(message_test);

        t1.get();
        t2.get();
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
