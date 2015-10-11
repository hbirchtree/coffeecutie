#include "coffee_impl/context/cdrenderer.h"

#include "plat/argument_parse.h"
#include "unit_tests/data_types.h"
#include "plat/environment_details.h"
#include "coffee_impl/audio/openal/copenal.h"

using namespace Coffee;
using namespace Coffee::CDisplay;
using namespace Coffee::CAudio;
using namespace Coffee::CAudio::COpenAL;

int main(int argc, char** argv)
{
    cDebug("Settings directory: %s",
           coffee_get_userdata_dir("hbirchtree","Best Coffee of All Time"));
    cDebug("Program directory:  %s",
           coffee_get_application_dir());
    cDebug("Launching from      %s",coffee_executable_name());

    Coffee::CoffeeInit();
    CoffeeTests::run_tests();

    CElapsedTimerMicro timer;
    timer.start();

    CResources::CResource rsc("/home/havard/Skrivebord/healing.ogg");
    rsc.read_data();

    CAudioSample smp;
    CStbAudio::coffee_stb_audio_vorbis_load(&smp,&rsc);

    CALContext ctxt;
    ctxt.callback = [](CALReport* r){
        cDebug("%s",r->message);
    };

    coffee_audio_context_create(&ctxt);
    coffee_audio_context_get_error(&ctxt);
    CALBuffer *buf = new CALBuffer;
    CALListener l;
    l.gain = 1;
    l.position = CVec3(0,0,0);
    l.velocity = CVec3(-100,0,-100);
    l.orientation_forward = CVec3(1,0,0);
    CALSource src;

    coffee_audio_listener_set(&l);
    coffee_audio_alloc(buf,&smp);
    rsc.free_data();
    free(smp.data);
    coffee_audio_alloc(&src);
    coffee_audio_source_transform(&src,CVec3(5,0,5),CVec3(10,0,0),CVec3(0,0,0));

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

    props.contextProperties.bits.alpha = 8;
    props.contextProperties.bits.red = 8;
    props.contextProperties.bits.blue = 8;
    props.contextProperties.bits.green = 8;
    props.contextProperties.bits.depth = 24;
    props.contextProperties.bits.stencil = 8;

    coffee_audio_source_queue_buffers(&src,1,&buf);
    coffee_audio_source_set_state(&src,CALStatePlaying);
    std::atomic<ubyte> atomic;
    atomic.store(0);
    CThreading::CThreadWorker<ubyte> worker(atomic);
//    std::future<void> ret = worker.run([=](){
        try{
            renderer->run(props);
        }catch(std::runtime_error exc){
            cDebug("Caught exception in thread: 0x%llx, message: %s",std::this_thread::get_id(),exc.what());
            worker.dataPtr()->store(1);
            renderer->cleanup();
        }
        if(worker.dataPtr()->load()==0)
            worker.dataPtr()->store(255);
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

    coffee_audio_context_destroy(&ctxt);

//    ret.get();

    delete renderer;
    return 0;
}
