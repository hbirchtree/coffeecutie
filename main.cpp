#include "coffee_impl/context/cdrenderer.h"

#include "plat/argument_parse.h"
#include "unit_tests/data_types.h"
#include "plat/environment_details.h"
#include "datasources/blam/cblam.h"
#include "coffee/cfiles.h"

using namespace Coffee;
using namespace Coffee::CDisplay;

int main(int argc, char** argv)
{
    cDebug("Settings directory: %s",
           coffee_get_userdata_dir("hbirchtree","Best Coffee of All Time"));
    cDebug("Program directory:  %s",
           coffee_get_application_dir());
    cDebug("Launching from      %s",coffee_executable_name());

    Coffee::CoffeeInit();
    CoffeeTests::run_tests();

    CResources::CResource h2("coagulation.map");
    h2.memory_map();
    const CBlam::blam_file_header* h2_map =
            CBlam::blam_file_header_get(h2.data,CBlam::blam_version_h2);

    const CBlam::blam_file_header_h2* h2_header = (const CBlam::blam_file_header_h2*)h2_map;

    CResources::CResource h1("bloodgulch.map");
    h1.memory_map();
    const CBlam::blam_file_header* h1_map =
            CBlam::blam_file_header_get(h1.data,CBlam::blam_version_pc);

    CResources::CResource h3("lockout.map");
    h3.memory_map();
    const CBlam::blam_file_header* h3_map = (const CBlam::blam_file_header*)h3.data;

    const CBlam::blam_file_header_h3* h3_header = (const CBlam::blam_file_header_h3*)h3_map;

    return 0;

    CElapsedTimerMicro timer;
    timer.start();

    CDRenderer* renderer = new CDRenderer(nullptr);

    //Magic happens here
    CDWindowProperties props;

    props.flags |=
            CDWindowProperties::Minimized |
            CDWindowProperties::Decorated |
            CDWindowProperties::Resizable |
            CDWindowProperties::Windowed;
    props.size.w = 1280;
    props.size.h = 720;
    props.monitor = 0;

    props.contextProperties.flags |=
            CGLContextProperties::GLDebug |
            CGLContextProperties::GLAutoResize;
    props.contextProperties.version.major = 3;
    props.contextProperties.version.minor = 3;

    props.contextProperties.bits.alpha = 8;
    props.contextProperties.bits.red = 8;
    props.contextProperties.bits.blue = 8;
    props.contextProperties.bits.green = 8;
    props.contextProperties.bits.depth = 24;
    props.contextProperties.bits.stencil = 8;

    std::atomic<ubyte> atomic;
    atomic.store(0);
    CThreading::CThreadWorker<ubyte> worker(atomic);
//    std::future<void> ret = worker.run([=](){
//        try{
            renderer->run(props);
//        }catch(std::runtime_error exc){
//            cDebug("Caught exception in thread: 0x%llx, message: %s",std::this_thread::get_id(),exc.what());
//            worker.dataPtr()->store(1);
//            renderer->cleanup();
//        }
//        if(worker.dataPtr()->load()==0)
//            worker.dataPtr()->store(255);
//    });

//    while(worker.dataPtr()->load()==0)
//        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    switch(worker.dataPtr()->load()){
    case 1:
        cDebug("Renderer exited with failure");
        break;
    default:
        cDebug("Renderer exited normally");
    }

    cDebug("Time: %lldus",timer.elapsed());

//    ret.get();

    delete renderer;
    return 0;
}
