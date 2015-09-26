#include "coffee_impl/context/cdrenderer.h"

#include "plat/argument_parse.h"
#include "unit_tests/data_types.h"

using namespace Coffee;
using namespace Coffee::CDisplay;

int main(int argc, char** argv)
{
    cDebug("Launching from %s",coffee_executable_name());

    Coffee::CoffeeInit();
    CoffeeTests::run_tests();

    CElapsedTimerMicro timer;
    timer.start();

    CDRenderer* renderer = new CDRenderer(nullptr);

    //Magic happens here
    CDWindowProperties props;

    props.flags |= CDWindowProperties::Minimized;
    props.flags |= CDWindowProperties::Resizable;
    props.flags |= CDWindowProperties::Windowed;
    props.flags |= CDWindowProperties::Decorated;
    props.size.w = 1280;
    props.size.h = 720;
    props.monitor = 0;

//    props.contextProperties.flags |= CGLContextProperties::GLVSync;
    props.contextProperties.flags |= CGLContextProperties::GLDebug;
    props.contextProperties.flags |= CGLContextProperties::GLAutoResize;
    props.contextProperties.version.major = 3;
    props.contextProperties.version.minor = 3;

    std::atomic<ubyte> atomic;
    atomic.store(0);
    CThreading::CThreadWorker<ubyte> worker(atomic);
    std::future<void> ret = worker.run<void>([=](){
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

    switch(worker.dataPtr()->load()){
    case 1:
        cDebug("Renderer exited with failure");
        break;
    default:
        cDebug("Renderer exited normally");
    }

    cDebug("Time: %lldus",timer.elapsed());

    ret.get();

    delete renderer;
    return 0;
}
