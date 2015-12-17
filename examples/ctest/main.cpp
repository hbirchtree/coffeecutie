#include <coffee/CCore>
#include <cdrenderer.h>

using namespace Coffee;
using namespace Coffee::CDisplay;

int32 coffee_main(int32,byte_t**)
{
    CResources::coffee_file_set_resource_prefix("sample_data/");

    CElapsedTimerMicro* timer = coffee_fun_alloc_timer_micro();
    timer->start();

    CDRenderer* renderer = new CDRenderer(nullptr);

    //Magic happens here
    CDWindowProperties props = coffee_get_default_visual();

    props.contextProperties.flags |= CGLContextProperties::GLDebug;

    std::atomic_bool status;
    std::future<void> ret = coffee_display_start_async(&status,renderer,props);

    cDebug("Launch time: {0}us, main thread is now free",timer->elapsed());

    ret.get();

    delete renderer;
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
