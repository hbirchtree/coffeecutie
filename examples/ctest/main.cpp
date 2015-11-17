#include "cdrenderer.h"

#include <coffee/core/plat/environment_details.h>
#include <coffee/core/plat/application_start.h>
#include <coffee/core/base/cfiles.h>

using namespace Coffee;
using namespace Coffee::CDisplay;

int32 coffee_main(int32,byte**)
{
    CResources::coffee_file_set_resource_prefix("sample_data/");

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
