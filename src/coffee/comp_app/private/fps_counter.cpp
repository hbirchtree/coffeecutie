#include <coffee/comp_app/fps_counter.h>

#include <coffee/core/debug/formatting.h>

namespace comp_app {

void FrameCounter::start_frame(ContainerProxy &, const time_point &current)
{
    get()++;

    if(next_print < current)
    {
        next_print = current + std::chrono::seconds(1);

        Coffee::cDebug("FPS: {0}", get());
        get() = 0;
    }
}

}
