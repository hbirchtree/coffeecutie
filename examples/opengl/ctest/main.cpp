#include <coffee/CCore>
#include <cdrenderer.h>

using namespace Coffee;
using namespace Coffee::CDisplay;

int32 coffee_main(int32,cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/");

    CElapsedTimerMicro timer;
    timer.start();

    CDRenderer* renderer = new CDRenderer(nullptr);

    //Magic happens here
    CDProperties props = GetDefaultVisual();

    props.gl.flags = props.gl.flags|
            GLProperties::GLDebug;

    std::future<void> ret = LaunchAsync(renderer,props);

    ret.get();

    delete renderer;
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
