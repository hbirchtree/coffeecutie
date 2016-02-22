#include <coffee/CCore>
#include <coffee/CImage>
#include "library.h"
#include <coffee/core/plat/memory/cmd_interface.h>

#include <coffee/core/unit_tests/micro_tests.h>

using namespace Coffee;

using namespace CLibraryLoader;

int32 coffee_main(int32, cstring_w*)
{
    {
        /* Check out system directory strings and user data directories */
        CString cfg_dir  = Env::GetUserData(
                    "hbirchtree",
                    "Best Coffee of All Time");

        CString app_dir  = Env::ApplicationDir();
        CString exe_name = Env::ExecutableName();

        cDebug("Settings directory: {0}",cfg_dir);
        cDebug("Program directory:  {0}",app_dir);
        cDebug("Launching from      {0}",exe_name);

        CResources::FileMkdir(cfg_dir.c_str(),true);
    }

//    CoffeeTests::FunctionCallTest();
//    CoffeeTests::PrintCallTest();

    cDebug("uint64 size: {0}",sizeof(uint64));
    cDebug("int64 size: {0}",sizeof(int64));
    cDebug("uint32 size: {0}",sizeof(uint32));
    cDebug("int32 size: {0}",sizeof(int32));

    cDebug("System memory: {0:1}GB",
           SysInfo::MemTotal()/CMath::pow<bigscalar>(1024,3));
    cDebug("Available system memory: {0:1}GB",
           SysInfo::MemAvailable()/CMath::pow<bigscalar>(1024,3));

    cDebug("Processor info: {0}",SysInfo::Processor());
    cDebug("Frequency: {0:2}GHz",SysInfo::ProcessorFrequency());
    cDebug("Hyper-threading: {0}",SysInfo::HasHyperThreading());
    cDebug("FPU: {0}",SysInfo::HasFPU());

    return 0;

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
        CObjectLoader<TestClass>* libtest = GetLib<TestClass>("test",&libver);

        if(libtest)
        {
            TestClass* impl = libtest->loader();
            impl->printHello();
            coffee_close_lib(libtest);
        }
    }

    {
        CResources::CResource res("test.file");
        res.size = 1024;
        res.data = Alloc(res.size);
        CResources::FileCommit(res,false);
        CResources::FileFree(res);
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
        uint8* d = (uint8*)Alloc(256*256);
        for(uint32 x=0;x<256;x++)
            for(uint32 y=0;y<256;y++)
            {
                d[y*255+x] =
                        NoiseGen::Linearize(NoiseGen::Perlin(CVec3(x,y,0)));
            }
        CStbImageLib::CStbImage img;
        img.data = d;
        img.bpp = 1;
        img.size.w = 255;
        img.size.h = 255;
        CResources::CResource resc("perlin.png");
        CStbImageLib::SavePNG(&resc,&img);
        CResources::FileCommit(resc);
        CResources::FileFree(resc);
        CFree(d);
    }

    CSize tsize = Cmd::TerminalSize();
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

    {
        cDebug("Testing multithread + printing, should be mutex-locked");
        std::function<void()> message_test = [](){
            int64 msgs = 0;
            ThreadId thread;

            while(msgs<10)
            {
                cDebug("{0}: Spam! Messages! Yes!",(const void*)thread.hash());
                msgs++;
            }
        };

        auto t1 = Threads::RunAsync(message_test);
        auto t2 = Threads::RunAsync(message_test);

        t1.get();
        t2.get();
    }

    {
        struct DataSet
        {
            CSize64 size;
            uint8* value;
        } data;
        data.size.w = 10240;
        data.size.h = 10240;
        data.value = (uint8*)Alloc(data.size.area());

        std::function<void(uint64,DataSet*)> kern = [](uint64 i, DataSet* d)
        {
            for(int32 j=0;j<64;j++)
                d->value[i*64+j] /= 2;
        };

        CElapsedTimer t;
        t.start();
        ParallelFor(kern,data.size.area()/64,&data).get();
        cDebug("Parallel time: {0}",t.elapsed());
        t.start();
        for(uint64 i=0;i<data.size.area();i++)
        {
            data.value[i] /= 2;
        }
        cDebug("Serial time: {0}",t.elapsed());
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
