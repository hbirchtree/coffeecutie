#include "cdrenderer.h"

#include <coffee/core/plat/environment_details.h>
#include <coffee/core/plat/application_start.h>
#include <coffee/core/base/cfiles.h>

using namespace Coffee;
using namespace Coffee::CDisplay;

int32 coffee_main(int32,byte**)
{
    Coffee::CoffeeInit();

    cstring_w cfg_dir = coffee_get_userdata_dir("hbirchtree","Best Coffee of All Time");
    cstring_w app_dir = coffee_get_application_dir();
    cstring_w exe_name = coffee_executable_name();

    cDebug("Settings directory: %s",cfg_dir);
    cDebug("Program directory:  %s",app_dir);
    cDebug("Launching from      %s",exe_name);

    CResources::coffee_file_mkdir(cfg_dir,true);

    free(cfg_dir);
    free(app_dir);
    free(exe_name);

    CElapsedTimerMicro* timer = coffee_fun_alloc_timer_micro();
    timer->start();

    CDRenderer* renderer = new CDRenderer(nullptr);

    //Magic happens here
    CDWindowProperties props = coffee_get_default_visual();

    props.contextProperties.flags |= CGLContextProperties::GLDebug;

    std::atomic<ubyte> atomic;
    atomic.store(0);
    CThreading::CThreadWorker<ubyte> worker(atomic);
    std::future<void> ret = worker.run([=](){
        try{
            renderer->run(props);
        }catch(std::runtime_error exc){
            cDebug("Caught exception in thread: 0x%llx, message: %s",std::this_thread::get_id(),exc.what());
            worker.dataPtr()->store(1);
            renderer->cleanup();
        }
        if(worker.dataPtr()->load()==0)
            worker.dataPtr()->store(255);
    });

    while(worker.dataPtr()->load()==0)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    cDebug((worker.dataPtr()->load() == 1)
           ? "Renderer exited with failure"
           : "Renderer exited normally");

    cDebug("Time: %lldus",timer->elapsed());

    ret.get();

    delete renderer;
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
