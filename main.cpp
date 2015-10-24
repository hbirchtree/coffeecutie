#include "coffee_impl/context/cdrenderer.h"

#include "plat/argument_parse.h"
#include "unit_tests/data_types.h"
#include "plat/environment_details.h"
#include "datasources/blam/cblam.h"
#include "coffee/cfiles.h"

using namespace Coffee;
using namespace Coffee::CDisplay;

int main(int,char**)
{
    cDebug("Haptic event: %i",sizeof(CIHapticEvent));
    cDebug("Sensor event: %i",sizeof(CISensorEvent));
    cDebug("CUAtomic event: %i",sizeof(CIControllerAtomicUpdateEvent));
    cDebug("CAtomic event: %i",sizeof(CIControllerAtomicEvent));

    cstring_w cfg_dir = coffee_get_userdata_dir("hbirchtree","Best Coffee of All Time");
    cstring_w app_dir = coffee_get_application_dir();
    cstring_w exe_name = coffee_executable_name();

    cDebug("Settings directory: %s",cfg_dir);
    cDebug("Program directory:  %s",app_dir);
    cDebug("Launching from      %s",exe_name);

    CResources::CFiles::coffee_file_mkdir(cfg_dir,true);

    free(cfg_dir);
    free(app_dir);
    free(exe_name);

    Coffee::CoffeeInit();
    CoffeeTests::run_tests();

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
