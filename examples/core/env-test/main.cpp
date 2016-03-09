#include <coffee/CCore>
#include <coffee/CImage>
#include <coffee/CGraphics>
#include "library.h"
#include <coffee/core/plat/memory/cmd_interface.h>
#include <coffee/core/datastorage/text/ini/ciniparser.h>

#include <coffee/core/unit_tests/micro_tests.h>

using namespace Coffee;

using namespace CLibraryLoader;

int32 coffee_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/");
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

        cDebug("Translation: {0}",get_translation(testmat));
    }

    {
//        CoffeeTests::FunctionCallTest();
//        CoffeeTests::PrintCallTest();
    }

    {
        cDebug("uint64 size: {0}",sizeof(uint64));
        cDebug("int64 size: {0}",sizeof(int64));
        cDebug("uint32 size: {0}",sizeof(uint32));
        cDebug("int32 size: {0}",sizeof(int32));
    }

    {
        cDebug("System memory: {0:1}GB",
               SysInfo::MemTotal()/CMath::pow<bigscalar>(1024,3));
        cDebug("Available system memory: {0:1}GB",
               SysInfo::MemAvailable()/CMath::pow<bigscalar>(1024,3));

        cDebug("Processor info: {0}",SysInfo::Processor());
        cDebug("Frequency: {0:2}GHz",SysInfo::ProcessorFrequency());
        cDebug("Hyper-threading: {0}",SysInfo::HasHyperThreading());
        cDebug("FPU: {0}",SysInfo::HasFPU());
    }

    /* Try creating an INI document */
    {
        CElapsedTimerMicro t;

        t.start();
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

        cDebug("Document creation: {0}",t.elapsed());

        t.start();
        CResources::CResource rsc("testoutfile.ini");
        cDebug("File object: {0}",t.elapsed());
        INI::Write(doc,rsc);
        cDebug("Document write: {0}",t.elapsed());
        CResources::FileCommit(rsc,false,ResourceAccess::Discard);
        cDebug("File save: {0}",t.elapsed());
        CResources::FileFree(rsc);
        cDebug("File free: {0}",t.elapsed());

    }

    /* Parsing an INI document */
    {
        CElapsedTimerMicro t;

        CResources::CResource testfile("test.ini");
        CResources::FileMap(testfile);

        t.start();
        INI::Document doc2 = INI::Read(testfile);
        cDebug("Parsing time: {0}",t.elapsed());

        CResources::CResource rsc("test.ini.ini");
        INI::Write(doc2,rsc);
        CResources::FileCommit(rsc);
        CResources::FileFree(rsc);

        CResources::FileUnmap(testfile);
    }

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
