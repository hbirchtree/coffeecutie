#include <coffee/CCore>
#include <coffee/CImage>
#include <coffee/CGraphics>
#include "library.h"
#include <coffee/core/plat/memory/cmd_interface.h>
#include <coffee/core/datastorage/text/ini/ciniparser.h>

#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/unit_tests/micro_tests.h>

using namespace Coffee;

using namespace CLibraryLoader;

int32 coffee_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/");

    Profiler::PushContext("Configuration data");
    {
        /* Check out system directory strings and user data directories */
        CString cfg_dir  = Env::GetUserData(
                    "hbirchtree",
                    "Best Coffee of All Time");
        Profiler::Profile("Get userdata directory");

        CString app_dir  = Env::ApplicationDir();
        CString exe_name = Env::ExecutableName();
        Profiler::Profile("Get application location");

        cDebug("Settings directory: {0}",cfg_dir);
        cDebug("Program directory:  {0}",app_dir);
        cDebug("Launching from      {0}",exe_name);
        Profiler::Profile("Print some data");

        CResources::FileMkdir(cfg_dir.c_str(),true);
        Profiler::Profile("Create directory recursively");
    }
    Profiler::PopContext();

    Profiler::PushContext("Matrix fun, part 1");
    {
        CGraphicsData::CTransform testt;
        testt.position = CVec3(3,3,3);
        testt.scale = CVec3(1);
        testt.rotation = CQuat(2,1,1.1,1);

        CMat4 testmat;
        testmat *= CGraphicsData::GenTransform(testt);
        testt.position = CVec3(3,2,3);
        testmat *= CGraphicsData::GenTransform(testt);
        testt.position = CVec3(3,3,2);
        testmat *= CGraphicsData::GenTransform(testt);

        CVec4 trans = get_translation(testmat);
        Profiler::Profile("Acquire matrix translation");

        cDebug("Translation: {0}",trans);
    }
    Profiler::PopContext();

    Profiler::PushContext("Mini-benchmarks");
    {
        //CoffeeTests::FunctionCallTest();
        Profiler::Profile("Function calls");
        //CoffeeTests::PrintCallTest();
        Profiler::Profile("Printing");
    }
    Profiler::PopContext();

    Profiler::PushContext("Size printing");
    {
        cDebug("uint64 size: {0}",sizeof(uint64));
        cDebug("int64 size: {0}",sizeof(int64));
        cDebug("uint32 size: {0}",sizeof(uint32));
        cDebug("int32 size: {0}",sizeof(int32));
    }
    Profiler::PopContext();

    Profiler::PushContext("System information");
    {
        cDebug("System memory: {0:1}GB",
               SysInfo::MemTotal()/CMath::pow<bigscalar>(1024,3));
        cDebug("Available system memory: {0:1}GB",
               SysInfo::MemAvailable()/CMath::pow<bigscalar>(1024,3));

        cDebug("Processor info: {0}",SysInfo::Processor());
        cDebug("Frequency: {0:2}GHz",SysInfo::ProcessorFrequency());
        cDebug("Hyper-threading: {0}",SysInfo::HasHyperThreading());
        cDebug("FPU: {0}",SysInfo::HasFPU());
        Profiler::Profile("Get system data");
    }
    Profiler::PopContext();

    {
        const uint32 testing_amount = 100;
        CElapsedTimerMicro t;

        /* Pushing context */
        t.start();
        for(uint32 i=0;i<testing_amount;i++)
            Profiler::PushContext("Test context");
        cDebug("Pushing context: {0}",t.elapsed());

        t.start();
        for(uint32 i=0;i<testing_amount;i++)
            Profiler::Profile("Value");
        cDebug("Profiling: {0}",t.elapsed());

        /* Popping context */
        t.start();
        for(uint32 i=0;i<testing_amount;i++)
            Profiler::PopContext();
        cDebug("Popping context: {0}",t.elapsed());
    }

    /* Try creating an INI document */
    Profiler::PushContext("INI document creation");
    {
        INI::Document doc;

        INI::Section t1 = doc.newSection();
        INI::Section t2 = doc.newSection();

        doc.insertSection("Test",t1);
        doc.insertSection("Rest",t2);

        INI::Variant v1 = doc.newString("Hello test string");
        INI::Variant v2 = doc.newBool(false);
        INI::Variant v3 = doc.newInteger(100);

        t1->insertValue("hello1",v1);
        t1->insertValue("hello2",v2);
        t1->insertValue("hello3",v3);

        Profiler::Profile("Creating and setting values");

        CResources::Resource rsc("testoutfile.ini");
        Profiler::Profile("File object");
        INI::Write(doc,rsc);
        Profiler::Profile("Writing object to file");
        CResources::FileCommit(rsc,false,ResourceAccess::Discard);
        Profiler::Profile("Committing file");
        CResources::FileFree(rsc);
        Profiler::Profile("Free'ing file");
    }
    Profiler::PopContext();

    /* Parsing an INI document */
    Profiler::PushContext("INI document parsing");
    {
        CResources::Resource testfile("test.ini");
        CResources::FileMap(testfile);
        Profiler::Profile("Mapping time");

        INI::Document doc2 = INI::Read(testfile);
        Profiler::Profile("Reading time");

        CResources::Resource rsc("test.ini.ini");
        INI::Write(doc2,rsc);
        CResources::FileCommit(rsc);
        CResources::FileFree(rsc);
        Profiler::Profile("Write-back");

        CResources::FileUnmap(testfile);
        Profiler::Profile("Unmapping");
    }
    Profiler::PopContext();

    return 0;

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
        /* Testing function slot/signal classes */
        CRect t(0,0,10,10);
        CFunctionSlot<CRect,int32> b(&t,&CRect::area);

        CFunctionSignal<CRect,int32> sigtest;
        cDebug("Signal return: {0}",sigtest.call(b));
    }

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
        Threads::ParallelFor(kern,data.size.area()/64,&data).get();
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
